#include "loader/demonsSoulsRenderSize.h"
#include "loader/demonsSoulsHdOutput.h"

#include "common/emulatorConfig.h"
#include "graphics/host_gpu/hostMemory.h"
#include "graphics/presentation/window.h"
#include "graphics/host_gpu/renderer/demonsSouls.h"
#include "loader/demonsSoulsSuperResolution.h"
#include "loader/elf.h"
#include "loader/guestCode.h"
#include "loader/runtimeLinker.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <optional>
#include <span>
#include <vector>

namespace Loader::DemonsSoulsRenderSize {
namespace {

bool Readable(uint64_t address, uint64_t size) {
	return Libs::Graphics::HostMemoryRangeIsReadable(address, size);
}

bool HoldsPair(uint64_t address, uint32_t width, uint32_t height) {
	uint32_t pair[2] {};
	if (!Readable(address, sizeof(pair))) return false;
	std::memcpy(pair, reinterpret_cast<const void*>(address), sizeof(pair));
	return pair[0] == width && pair[1] == height;
}

// The one address of a mode's render size the mode cases (`pattern`) load (both cases load the same constant), after
// their output (`output_width` x `output_height`: the 4K modes' unless given).
std::optional<uint64_t> FindRenderConstant(const Program& program, std::string_view pattern, uint32_t width,
                                           uint32_t height, uint32_t output_width = OutputWidth,
                                           uint32_t output_height = OutputHeight) {
	std::vector<uint64_t> constants;
	for (const auto site: GuestCode::Find(program, GuestCode::Pattern(pattern), 64)) {
		int32_t output_displacement = 0, render_displacement = 0;
		std::memcpy(&output_displacement, reinterpret_cast<const void*>(site + 4), sizeof(int32_t));
		std::memcpy(&render_displacement, reinterpret_cast<const void*>(site + 12),
		            sizeof(int32_t));
		const uint64_t output = site + 8 + static_cast<int64_t>(output_displacement);
		const uint64_t render = site + 16 + static_cast<int64_t>(render_displacement);
		if (HoldsPair(output, output_width, output_height) && HoldsPair(render, width, height) &&
		    std::find(constants.begin(), constants.end(), render) == constants.end())
			constants.push_back(render);
	}
	return constants.size() == 1 ? std::optional(constants[0]) : std::nullopt;
}

// The one place `bytes` is in the program's loaded segments.
std::optional<uint64_t> FindUniqueData(const Program& program, std::span<const uint8_t> bytes) {
	const auto* elf = program.elf.get();
	if (elf == nullptr || elf->GetEhdr() == nullptr || elf->GetPhdr() == nullptr)
		return std::nullopt;
	std::vector<uint64_t> found;
	const auto*           phdr = elf->GetPhdr();
	for (size_t i = 0; i < elf->GetEhdr()->e_phnum && found.size() < 2; i++) {
		if (phdr[i].p_type != PT_LOAD || phdr[i].p_filesz < bytes.size()) continue;
		const auto address = program.base_vaddr + phdr[i].p_vaddr;
		if (!Readable(address, phdr[i].p_filesz)) continue;
		const auto* begin = reinterpret_cast<const uint8_t*>(address);
		const auto* end   = begin + phdr[i].p_filesz;
		for (const auto* at = std::search(begin, end, bytes.begin(), bytes.end());
		     at != end && found.size() < 2;
		     at = std::search(at + 1, end, bytes.begin(), bytes.end()))
			found.push_back(reinterpret_cast<uint64_t>(at));
	}
	return found.size() == 1 ? std::optional(found[0]) : std::nullopt;
}

double g_wide_aspect = 0.0; // the aspect KYTY_RENDER_ASPECT installed (0: 16:9)
Size   g_output {OutputWidth, OutputHeight};

// The RIP-relative target of the disp32 at `site` + `at`, the instruction `length` bytes long.
uint64_t RipTarget(uint64_t site, uint32_t at, uint32_t length) {
	int32_t displacement = 0;
	std::memcpy(&displacement, reinterpret_cast<const void*>(site + at), sizeof(displacement));
	return site + length + static_cast<int64_t>(displacement);
}

// Mode 5 output in 01.007.000: discover all sites before staging changes (false, nothing
// changed, when one is not found):
// - the output pair its cases load. Display mode 4's (Cinematic: its 3D and output, natively) is
// the same
//   constant: its two loads are moved to the same 3840x2160 in the mode 3 preset (CinematicSource),
//   Cinematic kept as it is (rendered at 5120x2160 natively the game ran out of its memory pools
//   and stopped);
// - the upscaler's context (SuperResolution::InitContext's 3840, 2160);
// - the fixed 3840x2160 the video settings' three record builders store, which sizes the upscaler's
// image and the
//   processing after it: replace only its imm64 size. The following display/UI size
//   is deliberately independent, and its original mode pair and tables stay 4K;
// - validate the original output width and height tables even though they stay intact.
bool PrepareOutput(const Program& program, Size output_size, GuestCode::PatchBatch& patches) {
	std::vector<uint64_t>
	    outputs; // (the setter's and the constructor's mode 5 cases load the same pair)
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
	return std::clamp(height, 720u, upscaler ? OutputHeight : RenderHeight);
}

static bool InstallSettings(Program* program, bool trinity) {
	const char* setting = std::getenv("KYTY_RENDER_HEIGHT");
	const char* wide    = std::getenv("KYTY_RENDER_ASPECT");
	const bool  numeric   = setting != nullptr && *setting != 0 && std::strcmp(setting, "auto") != 0;
	const auto  automatic = AutoHeight(DemonsSoulsSuperResolution::Active());
	if ((!trinity && !numeric && automatic == 0 && (wide == nullptr || *wide == 0)) ||
	    program == nullptr || !Libs::Graphics::DemonsSouls::IsSupportedGame() ||
	    program->file_name.filename() != "eboot.bin")
		return false;
	const bool     hd          = DemonsSoulsHdOutput::Active();
	const uint32_t game_width  = hd ? DemonsSoulsHdOutput::HdWidth : trinity ? TrinityWidth : RenderWidth;
	const uint32_t game_height = hd ? DemonsSoulsHdOutput::HdHeight : trinity ? TrinityHeight : RenderHeight;
	const int      mode        = hd ? HdUpscaledMode : trinity ? TrinityMode : PerformanceMode;
	const uint32_t out_width   = hd ? DemonsSoulsHdOutput::HdWidth : OutputWidth;
	const uint32_t out_height  = hd ? DemonsSoulsHdOutput::HdHeight : OutputHeight;
	auto requested = automatic != 0 ? automatic
	                 : numeric      ? static_cast<uint32_t>(std::strtoul(setting, nullptr, 10))
	                                : game_height;
	if (automatic != 0) {
		uint32_t window_width = 0, window_height = 0;
		Libs::Graphics::WindowOutputSize(&window_width, &window_height);
		std::printf("Demon's Souls render size: automatic for the %ux%u window: %s 3D at height %u%s\n", window_width,
		            window_height, hd ? "HD output (1920x1080)," : trinity ? "PS5 Pro output (3840x2160)," : "4K output,",
		            std::min(requested, hd ? DemonsSoulsHdOutput::HdHeight : requested),
		            hd && window_height == DemonsSoulsHdOutput::HdHeight ? " (pixel-exact)"
		            : !hd && !trinity && window_height == OutputHeight ? " (pixel-exact)"
		                                                               : "");
		if (hd && requested > DemonsSoulsHdOutput::HdHeight) requested = DemonsSoulsHdOutput::HdHeight;
	}
	if (hd && requested > DemonsSoulsHdOutput::HdHeight && requested <= OutputHeight) {
		std::printf("Demon's Souls render size: KYTY_RENDER_HEIGHT=%u is above the HD output's 1080; 1080 instead\n",
		            requested);
		requested = DemonsSoulsHdOutput::HdHeight;
	}
	const auto aspect = wide != nullptr && *wide != 0 ? ParseAspect(wide) : 0.0;
	if (wide != nullptr && *wide != 0 && aspect == 0.0)
		std::printf("Demon's Souls render size: KYTY_RENDER_ASPECT=%s is not wider than 16:9 and "
		            "at most 32:9; 16:9 kept\n",
		            wide);
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
			            "the game's own TAA kept\n",
			            Config::GetScreenWidth(), Config::GetScreenHeight());
			return false;
		}
		if (requested >= 720 && requested <= OutputHeight) {
			const auto clamped = SuperResolutionRenderHeight(requested, output);
			if (clamped != requested)
				std::printf("Demon's Souls render size: requested %u lines clamped to %u for the "
				            "%ux%u DLSS output\n",
				            requested, clamped, output.width, output.height);
			requested = clamped;
		}
	}
	if (!trinity && requested == game_height && aspect == 0.0) return true;
	auto size = RenderSizeFor(requested, aspect != 0.0 ? aspect : GameAspect);
	if (const auto most = MaxRenderHeight(aspect != 0.0 ? aspect : GameAspect); size.height > most) {
		const auto lower = RenderSizeFor(most, aspect != 0.0 ? aspect : GameAspect);
		std::printf("Demon's Souls render size: %ux%u does not fit the game's render target memory (made for 3840x2160: "
		            "a red or broken picture, or a crash); %ux%u instead\n",
		            size.width, size.height, lower.width, lower.height);
		size = lower;
	}
	if (size.height == 0) {
		std::printf("Demon's Souls render size: KYTY_RENDER_HEIGHT=%s is not 720-2160; the game's "
		            "%ux%u kept\n",
		            setting, game_width, game_height);
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
		            "game's %ux%u kept\n",
		            game_width, game_height);
		return false;
	}
	GuestCode::PatchBatch patches;
	if (trinity && !PrepareOutput(*program, output, patches)) {
		std::printf("Demon's Souls super resolution: complete output patch not found; the game's "
		            "own TAA kept\n");
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
	if (trinity) std::printf("Demon's Souls canvases: SR/processing %ux%u, display/interface %ux%u; interface composed once before presentation\n",
	                        output.width, output.height, OutputWidth, OutputHeight);
	std::printf("Demon's Souls render size: %s mode renders %ux%u instead of %ux%u (output %ux%u)%s\n",
	            hd ? "HD" : "performance", size.width, size.height, game_width, game_height,
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
}

} // namespace Loader::DemonsSoulsRenderSize
