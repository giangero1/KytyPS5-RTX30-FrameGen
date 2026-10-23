#include "demonsSoulsRenderSize.h"

#include "demonsSoulsHdOutput.h"
#include "demonsSoulsSuperResolution.h"
#include "emulator/src/graphics/guest_gpu/superResolution.h"
#include "emulator/src/libs/graphics.h"
#include "guestCode.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace Loader::DemonsSoulsRenderSize {

namespace {
double g_wide_aspect = 0.0;
Size   g_output {OutputWidth, OutputHeight};

std::optional<uint64_t> FindUniqueData(const Program& program, std::span<const uint8_t> bytes) {
	const auto sites = GuestCode::Find(program, GuestCode::Pattern(bytes), 8);
	if (sites.size() != 1) return std::nullopt;
	return sites[0];
}

bool HoldsPair(uint64_t site, uint32_t first, uint32_t second) {
	uint32_t pair[2] {};
	std::memcpy(pair, reinterpret_cast<const void*>(site), sizeof(pair));
	return pair[0] == first && pair[1] == second;
}

std::optional<uint64_t> FindRenderConstant(const Program& program, std::string_view pattern,
                                           uint32_t width, uint32_t height,
                                           uint32_t out_width = OutputWidth,
                                           uint32_t out_height = OutputHeight) {
	const auto sites = GuestCode::Find(program, GuestCode::Pattern(pattern), 8);
	for (const auto site: sites) {
		int32_t out_disp = 0, render_disp = 0;
		std::memcpy(&out_disp, reinterpret_cast<const void*>(site + 4), sizeof(out_disp));
		std::memcpy(&render_disp, reinterpret_cast<const void*>(site + 12), sizeof(render_disp));
		const uint64_t out_site    = site + 8 + static_cast<int64_t>(out_disp);
		const uint64_t render_site = site + 16 + static_cast<int64_t>(render_disp);
		if (HoldsPair(out_site, out_width, out_height) && HoldsPair(render_site, width, height))
			return render_site;
	}
	return std::nullopt;
}

uint64_t RipTarget(uint64_t site, uint32_t at, uint32_t length) {
	int32_t displacement = 0;
	std::memcpy(&displacement, reinterpret_cast<const void*>(site + at), sizeof(displacement));
	return site + length + static_cast<int64_t>(displacement);
}

// Mode 5 output in 01.007.000: discover all sites before staging changes (false, nothing
// changed, when one is not found):
// - the output pair its cases load. Display mode 4's (Cinematic: its 3D and output, natively) is the same
//   constant: its two loads are moved to the same 3840x2160 in the mode 3 preset (CinematicSource),
//   Cinematic kept as it is (rendered at 5120x2160 natively the game ran out of its memory pools and stopped);
// - the upscaler's context (SuperResolution::InitContext's 3840, 2160);
// - the fixed 3840x2160 the video settings' three record builders store, which sizes the upscaler's image and the
//   processing after it: replace only its imm64 size. The following display/UI size
//   is deliberately independent, and its original mode pair and tables stay 4K;
// - validate the original output width and height tables even though they stay intact.
bool PrepareOutput(const Program& program, Size output_size, GuestCode::PatchBatch& patches) {
	std::vector<uint64_t> outputs; // (the setter's and the constructor's mode 5 cases load the same pair)
	for (const auto site: GuestCode::Find(program, GuestCode::Pattern(TrinityCase), 8)) {
		const uint64_t output = RipTarget(site, 4, 8);
		if (HoldsPair(output, OutputWidth, OutputHeight) &&
		    std::find(outputs.begin(), outputs.end(), output) == outputs.end())
			outputs.push_back(output);
	}
	if (outputs.size() != 1) return false;
	std::vector<uint64_t> cinematic;
	for (const auto site: GuestCode::Find(program, GuestCode::Pattern(CinematicCase), 8))
		if (RipTarget(site, 4, 8) == outputs[0]) cinematic.push_back(site);
	const auto preset =
	    FindUniqueData(program, std::span(reinterpret_cast<const uint8_t*>(CinematicSource),
	                                      sizeof(CinematicSource)));
	const auto contexts = GuestCode::Find(program, GuestCode::Pattern(UpscalerContext), 2);
	const auto setter   = GuestCode::Find(program, GuestCode::Pattern(SetterFixedPair), 2);
	const auto builder  = GuestCode::Find(program, GuestCode::Pattern(ConstructorFixedPair), 2);
	const auto query    = GuestCode::Find(program, GuestCode::Pattern(QueryFixedPair), 2);
	const auto widths   = FindUniqueData(
        program, std::span(reinterpret_cast<const uint8_t*>(OutputWidths), sizeof(OutputWidths)));
	const auto heights = FindUniqueData(
	    program, std::span(reinterpret_cast<const uint8_t*>(OutputHeights), sizeof(OutputHeights)));
	if (cinematic.size() != 2 || !preset || contexts.size() != 1 || setter.size() != 1 ||
	    builder.size() != 1 || query.size() != 1 || !widths || !heights)
		return false;
	// (The 3840x2160 after the preset's 0x80ffffff and 2560x1440; nothing writes the preset.)
	const uint64_t source = *preset + 12;
	for (const auto site: cinematic) {
		const auto displacement =
		    static_cast<int32_t>(static_cast<int64_t>(source) - static_cast<int64_t>(site + 8));
		patches.Value(site + 4, displacement);
	}
	// Two independent fields: record +0x0c is the SR/processing size; +0x14
	// is the display/interface size. Keep display tables and the shared mode pair
	// at 4K so UI allocations, projection and fixed-pixel layout remain coherent.
	patches.Value(contexts[0] + 1, output_size.width);
	patches.Value(contexts[0] + 6, output_size.height);
	const uint64_t processing_pair = output_size.width | (uint64_t {output_size.height} << 32);
	patches.Value(setter[0] + 6, processing_pair);
	patches.Value(builder[0] + 9, processing_pair);
	patches.Value(query[0] + 5, processing_pair);
	return true;
}

} // namespace

double WideAspect() {
	return g_wide_aspect;
}

uint32_t WideOutputWidth() {
	return g_output.width != OutputWidth || g_output.height != OutputHeight ? g_output.width : 0;
}

Size EffectiveOutputSize() {
	return g_output;
}

uint32_t AutoHeight(bool upscaler) {
	const char* setting = std::getenv("KYTY_RENDER_HEIGHT");
	if (setting != nullptr && *setting != 0 && std::strcmp(setting, "auto") != 0) return 0;
	uint32_t width = 0, height = 0;
	Libs::Graphics::WindowOutputSize(&width, &height);
	if (height == 0) return 0;
	if (upscaler) height = std::max(SuperResolutionLeastHeight, static_cast<uint32_t>(std::lround(height / 1.5)));
	return std::clamp(height, LeastRenderHeight, upscaler ? OutputHeight : RenderHeight);
}

static bool InstallSettings(Program* program, bool trinity) {
	const char* setting = std::getenv("KYTY_RENDER_HEIGHT");
	const char* wide    = std::getenv("KYTY_RENDER_ASPECT");
	const bool  numeric   = setting != nullptr && *setting != 0 && std::strcmp(setting, "auto") != 0;
	const auto  automatic = AutoHeight(DemonsSoulsSuperResolution::Active());
	const bool  hd_pro    = DemonsSoulsSuperResolution::Active() && DemonsSoulsSuperResolution::HdOutput();
	const bool  hd        = DemonsSoulsHdOutput::Active() && !hd_pro;
	if ((!trinity && !numeric && automatic == 0 && !hd_pro && !hd && (wide == nullptr || *wide == 0)) ||
	    program == nullptr || !Libs::Graphics::DemonsSouls::IsSupportedGame() ||
	    program->file_name.filename() != "eboot.bin")
		return false;
	const uint32_t game_width  = hd ? DemonsSoulsHdOutput::HdWidth : trinity ? TrinityWidth : RenderWidth;
	const uint32_t game_height = hd ? DemonsSoulsHdOutput::HdHeight : trinity ? TrinityHeight : RenderHeight;
	const int      mode        = hd ? HdUpscaledMode : trinity ? TrinityMode : PerformanceMode;
	const uint32_t out_width   = hd ? DemonsSoulsHdOutput::HdWidth : OutputWidth;
	const uint32_t out_height  = hd ? DemonsSoulsHdOutput::HdHeight : OutputHeight;
	auto requested = automatic != 0 ? automatic
	                 : numeric      ? static_cast<uint32_t>(std::strtoul(setting, nullptr, 10))
	                                : game_height;
	if (hd_pro && requested > DemonsSoulsHdOutput::HdHeight) {
		std::printf("Demon's Souls render size: %u is above the 1080p output's 1080 (the HD output with the upscaler); "
		            "1080 instead
",
		            requested);
		requested = DemonsSoulsHdOutput::HdHeight;
	}
	if (automatic != 0) {
		uint32_t window_width = 0, window_height = 0;
		Libs::Graphics::WindowOutputSize(&window_width, &window_height);
		std::printf("Demon's Souls render size: automatic for the %ux%u window: %s 3D at height %u%s
", window_width,
		            window_height, hd ? "HD output (1920x1080)," : trinity ? "PS5 Pro output (3840x2160)," : "4K output,",
		            std::min(requested, hd ? DemonsSoulsHdOutput::HdHeight : requested),
		            hd && window_height == DemonsSoulsHdOutput::HdHeight ? " (pixel-exact)"
		            : !hd && !trinity && window_height == OutputHeight ? " (pixel-exact)"
		                                                               : "");
		if (hd && requested > DemonsSoulsHdOutput::HdHeight) requested = DemonsSoulsHdOutput::HdHeight;
	}
	if (hd && requested > DemonsSoulsHdOutput::HdHeight && requested <= OutputHeight) {
		std::printf("Demon's Souls render size: KYTY_RENDER_HEIGHT=%u is above the HD output's 1080; 1080 instead
",
		            requested);
		requested = DemonsSoulsHdOutput::HdHeight;
	}
	auto aspect = wide != nullptr && *wide != 0 ? ParseAspect(wide) : 0.0;
	if (wide != nullptr && *wide != 0 && aspect == 0.0)
		std::printf("Demon's Souls render size: KYTY_RENDER_ASPECT=%s is not wider than 16:9 and "
		            "at most 32:9; 16:9 kept
",
		            wide);
	if (hd_pro && aspect != 0.0) {
		std::printf("Demon's Souls render size: a wider aspect is not with the 1080p output (the HD output with the "
		            "upscaler); 16:9
");
		aspect = 0.0;
	}
	Size output {OutputWidth, OutputHeight};
	if (trinity) {
		const char*  wide_output = std::getenv("KYTY_WIDE_OUTPUT");
		const double output_aspect =
		    aspect != 0.0 && (wide_output == nullptr || std::strcmp(wide_output, "0") != 0)
		        ? aspect
		        : GameAspect;
		output = OutputSizeFor(Config::GetScreenWidth(), Config::GetScreenHeight(), output_aspect);
		if (output.height == 0) {
			std::printf("Demon's Souls super resolution: selected output %ux%u cannot fit a "
			            "1080-line PS5 Pro canvas; "
			            "the game's own TAA kept
",
			            Config::GetScreenWidth(), Config::GetScreenHeight());
			return false;
		}
		if (requested >= LeastRenderHeight && requested <= OutputHeight) {
			const auto clamped = SuperResolutionRenderHeight(requested, output);
			if (clamped != requested)
				std::printf("Demon's Souls render size: requested %u lines clamped to %u for the "
				            "%ux%u DLSS output
",
				            requested, clamped, output.width, output.height);
			requested = clamped;
		}
	}
	if (!hd && !trinity && requested == game_height && aspect == 0.0) return true;
	auto size = RenderSizeFor(requested, aspect != 0.0 ? aspect : GameAspect);
	if (const auto most = MaxRenderHeight(aspect != 0.0 ? aspect : GameAspect); size.height > most) {
		const auto lower = RenderSizeFor(most, aspect != 0.0 ? aspect : GameAspect);
		std::printf("Demon's Souls render size: %ux%u does not fit the game's render target memory (made for 3840x2160: "
		            "a red or broken picture, or a crash); %ux%u instead
",
		            size.width, size.height, lower.width, lower.height);
		size = lower;
	}
	if (size.height == 0) {
		std::printf("Demon's Souls render size: KYTY_RENDER_HEIGHT=%s is not %u-2160; the game's "
		            "%ux%u kept
",
		            setting, LeastRenderHeight, game_width, game_height);
		return false;
	}
	auto constant = hd ? FindRenderConstant(*program, ModeCase, ModeWidths[mode], ModeHeights[mode], out_width, out_height)
	                   : FindRenderConstant(*program, trinity ? TrinityCase : ModeCase, game_width, game_height);
	if (!constant && !trinity && !hd) constant = FindRenderConstant(*program, ModeCaseDuplicated, game_width, game_height);
	const auto widths   = FindUniqueData(*program, std::span(reinterpret_cast<const uint8_t*>(ModeWidths), sizeof(ModeWidths)));
	const auto heights  = FindUniqueData(*program, std::span(reinterpret_cast<const uint8_t*>(ModeHeights), sizeof(ModeHeights)));
	// (The render setup's 16:9: the one such store in the code.)
	std::optional<uint64_t> aspect_store;
	if (aspect != 0.0) {
		const auto sites = GuestCode::Find(*program, GuestCode::Pattern(AspectStore), 2);
		if (sites.size() == 1) aspect_store = sites[0];
	}
	if (!constant || !widths || !heights || (aspect != 0.0 && !aspect_store)) {
		std::printf("Demon's Souls render size: the video settings of a known build not found; the "
		            "game's %ux%u kept
",
		            game_width, game_height);
		return false;
	}
	GuestCode::PatchBatch patches;
	if (trinity && !PrepareOutput(*program, output, patches)) {
		std::printf("Demon's Souls super resolution: complete output patch not found; the game's "
		            "own TAA kept
");
		return false;
	}
	// Installation occurs before module initializers or guest threads run; program memory is
	// writable to the host.
	const uint32_t pair[2] {size.width, size.height};
	patches.Value(*constant, pair);
	patches.Value(*widths + mode * sizeof(uint32_t), size.width);
	patches.Value(*heights + mode * sizeof(uint32_t), size.height);
	if (hd) DemonsSoulsHdOutput::SetMode(HdUpscaledMode + 1);
	if (aspect_store) {
		// The projection's aspect: the render size's own, as the game's 2560x1440 has 16/9.
		const float value = static_cast<float>(static_cast<double>(size.width) / size.height);
		patches.Value(*aspect_store + 3, value);
	}
	if (!patches.Apply()) return false;
	if (aspect_store) g_wide_aspect = static_cast<double>(size.width) / size.height;
	if (trinity) g_output = output;
	if (trinity && (hd_pro || size.height < SuperResolutionLeastHeight)) {
		(void)DemonsSoulsSuperResolution::SetLeastHeight(program, size.height);
	}
	if (trinity) std::printf("Demon's Souls canvases: SR/processing %ux%u, display/interface %ux%u; interface composed once before presentation
",
	                        output.width, output.height, OutputWidth, OutputHeight);
	std::printf("Demon's Souls render size: %s mode renders %ux%u instead of %ux%u (output %ux%u)%s
",
	            hd ? "HD" : "performance", size.width, size.height, hd ? ModeWidths[mode] : game_width,
	            hd ? ModeHeights[mode] : game_height,
	            output.width, output.height,
	            trinity        ? ", DLSS output follows launcher resolution"
	            : aspect_store ? ", its projection as wide"
	                           : "");
	return true;
}

bool InstallSuperResolution(Program* program) {
	return InstallSettings(program, true);
}

void Install(Program* program) {
	if (!DemonsSoulsSuperResolution::Active()) (void)InstallSettings(program, false);
}

} // namespace Loader::DemonsSoulsRenderSize
