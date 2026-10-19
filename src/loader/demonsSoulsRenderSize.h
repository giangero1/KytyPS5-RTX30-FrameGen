#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <span>
#include <string_view>

namespace Loader {
struct Program;
// Demon's Souls performance-mode render size (KYTY_RENDER_HEIGHT) and aspect (KYTY_RENDER_ASPECT).
// The original PS5/TAA path keeps its 3840x2160 output. DLSS (mode 5 in 01.007.000)
// fits its SR/scene processing output to Config's launcher resolution, at 1080..2160
// lines, and clamps its render height to that output. The display/interface canvas
// retains its original 3840x2160 size and is composed once before presentation.
// All original constants below remain unchanged for guest signature matching.
// Wider aspects (21:9 means 64:27, at most 32:9) use the matching projection. With
// DLSS, KYTY_WIDE_OUTPUT=0 retains a 16:9 output, squeezed as on the original TAA
// path; otherwise the SR output matches the requested aspect. The original 4K UI
// follows the existing ultrawide anchoring path, without reduced-height transforms.
namespace DemonsSoulsRenderSize {
void Install(Program* program);
// Discover and install mode 5's render/output settings before enabling the PS5 Pro path.
// False means no guest memory was changed; the caller must keep the game's TAA.
[[nodiscard]] bool InstallSuperResolution(Program* program);
// The aspect the 3D is rendered at when KYTY_RENDER_ASPECT took effect (Install), else 0 (the
// game's 16:9).
[[nodiscard]] double WideAspect();
// Compatibility accessor: changed output width, or 0 for the original 3840x2160 canvas.
[[nodiscard]] uint32_t WideOutputWidth();

// The video settings' mode 3 case (the setter's and the constructor's): the output size, then the render size, as
// RIP-relative pairs of 32-bit width and height (vmovsd), then a short jump (01.007.000); 01.005.000 loads the same
// pairs with vmovddup (0x8eea7b, 0x8ef5e5), which found no case there before (issue #69: its render height kept).
inline constexpr std::string_view ModeCase           = "c5 fb 10 0d ?? ?? ?? ?? c5 fb 10 05 ?? ?? ?? ?? eb";
inline constexpr std::string_view ModeCaseDuplicated = "c5 fb 12 0d ?? ?? ?? ?? c5 fb 12 05 ?? ?? ?? ?? eb";
inline constexpr uint32_t         OutputWidth = 3840, OutputHeight = 2160, RenderWidth = 2560, RenderHeight = 1440;
// Its mode 5 case (the PS5 Pro's, with super resolution: loader/demonsSoulsSuperResolution.h): the same pairs, then
// the super resolution flag set, then the jump (01.007.000).
inline constexpr std::string_view TrinityCase = "c5 fb 10 0d ?? ?? ?? ?? c5 fb 10 05 ?? ?? ?? ?? c6 45 ?? 01 eb";
inline constexpr uint32_t         TrinityWidth = 2688, TrinityHeight = 1512;
// The least render height the game's super resolution runs at (SuperResolution::IsEnabled).
inline constexpr uint32_t SuperResolutionLeastHeight = 1080;
// The wide output's places (01.007.000, DemonsSoulsRenderSize.cpp PrepareOutput):
// - display mode 4's cases (Cinematic), loading its 3D size and output from the modes' shared
// 3840x2160 output pair:
//   vmovsd xmm0, [rip + output]; jmp; and the next case's vmovsd xmm1 (0x90090a66d, 0x90090b1d2;
//   mode 3's look alike but load another pair), and the 3840x2160 they load instead: in the mode 3
//   preset (0x90237f260);
// - the upscaler's context, SuperResolution::InitContext(3840, 2160): mov esi, 3840; mov edx, 2160;
// call (0x900a521eb);
// - the video settings records' fixed 3840x2160 (mov r64, 2160 << 32 | 3840) that sizes the
// upscaler's image and the
//   processing after it, in the setter (after vmovaps xmm1, xmm0: 0x90090a6ab), the constructor
//   (after mov r14, [rip + x]: 0x90090b217) and the size query (after mov [rsi + 4], edi:
//   0x90090ac3f);
// - the per-mode output widths after the render size tables (ModeWidths).
inline constexpr std::string_view CinematicCase = "c5 fb 10 05 ?? ?? ?? ?? eb ?? c5 fb 10 0d";
inline constexpr uint32_t         CinematicSource[8] {0x80ffffffu, 2560, 1440, 3840,
                                                      2160,        3840, 2160, 1024};
inline constexpr std::string_view UpscalerContext = "be 00 0f 00 00 ba 70 08 00 00 e8";
inline constexpr std::string_view SetterFixedPair =
    "c5 f8 28 c8 48 b9 00 0f 00 00 70 08 00 00 48 ba";
inline constexpr std::string_view ConstructorFixedPair =
    "4c 8b 35 ?? ?? ?? ?? 48 b9 00 0f 00 00 70 08 00 00";
inline constexpr std::string_view QueryFixedPair = "89 7e 04 48 ba 00 0f 00 00 70 08 00 00 48 bf";
inline constexpr uint32_t         OutputWidths[5] {1920, 1920, 3840, 3840, 3840};
inline constexpr uint32_t         OutputHeights[5] {1080, 1080, 2160, 2160, 2160};
// The UI canvas and display mode float resolution tables in 01.007.000:
// - CanvasFloatPair at 0x24fb510: the unique (3840.0f, 2160.0f) loaded by renderer setup (0x9e6f7c)
//   and stored by UI setup (0x10447fa) into the global UI canvas size (0x32b5538);
// - FloatHeights at 0x24fdc10: the mode output heights {1080.0f, 2160.0f};
// - FloatWidths at 0x24fdc88: the mode output widths {1920.0f, 3840.0f}.
inline constexpr float            CanvasFloatPair[2] {3840.0f, 2160.0f};
inline constexpr float            FloatHeights[2] {1080.0f, 2160.0f};
inline constexpr float            FloatWidths[2] {1920.0f, 3840.0f};
// The render size query's per-mode tables (modes 1-5): widths, heights.
inline constexpr uint32_t ModeWidths[5] {1600, 1920, 2560, 3840, 2688};
inline constexpr uint32_t ModeHeights[5] {900, 1080, 1440, 2160, 1512};
inline constexpr int      PerformanceMode = 2, TrinityMode = 4; // (indexes in the tables)
// The render setup's constructor storing the projection's aspect, 16/9 as a float: mov dword
// [rbx+4], 0x3fe38e39 (01.007.000 0x90090ae7f, 01.005.000 0x9008ef287; the only such store either
// build has).
inline constexpr std::string_view AspectStore  = "c7 43 04 39 8e e3 3f";
inline constexpr double           GameAspect   = 16.0 / 9.0;
inline constexpr double           WidestAspect = 32.0 / 9.0;

// The render size for a height and an aspect, its width a multiple of 8; {0, 0} for a height out of
// range.
struct Size {
	uint32_t width  = 0;
	uint32_t height = 0;
};
// Effective PS5 Pro SR/processing output; display/interface constants remain 4K.
[[nodiscard]] Size EffectiveOutputSize();

// Fit the game aspect inside the selected launcher resolution, retaining the game's 8-pixel width
// alignment. Below 1080 the PS5 Pro upscaler cannot run; the caller keeps the original TAA path.
inline Size OutputSizeFor(uint32_t screen_width, uint32_t screen_height,
                          double aspect = GameAspect) {
	if (screen_width == 0 || screen_height == 0 || !std::isfinite(aspect) || aspect < GameAspect ||
	    aspect > WidestAspect + 0.01)
		return {};
	uint32_t height = std::min(screen_height, OutputHeight);
	while (height >= SuperResolutionLeastHeight) {
		const auto width = static_cast<uint32_t>(std::lround(height * aspect / 8.0)) * 8u;
		if (width <= screen_width) return {width, height};
		--height;
	}
	return {};
}

inline uint32_t SuperResolutionRenderHeight(uint32_t requested, Size output) {
	return output.height >= SuperResolutionLeastHeight
	           ? std::clamp(requested, SuperResolutionLeastHeight, output.height)
	           : 0;
}

// Transform from the game's fixed interface canvas to its output. HUD elements retain
// their proportions on ultrawide outputs and are anchored at the requested side.
struct InterfaceTransform {
	float xscale = 1.0f, yscale = 1.0f, xshift = 0.0f;
	void  InverseMask(float (&row)[4]) const {
        row[0] /= xscale;
        row[1] /= yscale;
        row[3] -= row[0] * xshift;
	}
};

// Copy quads for video can already cover the native viewport, whereas menu icons
// use the game's fixed 4K layout. Only an exact normalized fullscreen quad skips scaling.
inline bool NativeFullscreenQuad(std::span<const std::array<float, 2>> vertices) {
	if (vertices.size() != 4 && vertices.size() != 6) return false;
	bool left = false, right = false, top = false, bottom = false;
	for (const auto& xy: vertices) {
		if (!std::isfinite(xy[0]) || !std::isfinite(xy[1]) ||
		    std::fabs(std::fabs(xy[0]) - 1.0f) > 0.001f ||
		    std::fabs(std::fabs(xy[1]) - 1.0f) > 0.001f) return false;
		left |= xy[0] < 0; right |= xy[0] > 0;
		top |= xy[1] < 0; bottom |= xy[1] > 0;
	}
	return left && right && top && bottom;
}
inline InterfaceTransform InterfaceTransformFor(Size output, bool hud = false, float side = 0.0f,
                                                Size layout = {OutputWidth, OutputHeight}) {
	const float y = static_cast<float>(output.height) / layout.height;
	const float x = static_cast<float>(output.width) / layout.width;
	return {hud ? y : x, y, hud ? (output.width - layout.width * y) * (side + 1.0f) * 0.5f : 0.0f};
}
inline Size RenderSizeFor(uint32_t height, double aspect = GameAspect) {
	if (height < 720 || height > OutputHeight) return {};
	const auto width = static_cast<uint32_t>(std::lround(height * aspect / 8.0)) * 8u;
	return {width, height};
}

// Whether a 3D of `size` fits the game's render target memory, made for its own largest 3D, the Cinematic mode's
// 3840x2160: its targets laid out in the GPU's 64 KiB tiles (at 2, 4, 8 and 16 bytes a pixel: 256x128, 128x128, 128x64
// and 64x64 pixels) take no more tiles than at 3840x2160. A target taking more writes past its memory into what
// follows: 4416x1864 at 21:9 (525 tiles at 4 bytes, 3840x2160 510) showed the 3D in red only, 4776x2016 and 32:9's
// 5760x1620 in yellow with black models, 4952x2088 and 5120x2160 ended the game at start-up (eboot+0x803e3a, a tree
// of its own whose node was overwritten); 4264x1800 (510 tiles) and 5120x1440 are right (the RTX 5090, 01.007.000, the
// game's TAA). The pixels alone are no measure: 4416x1864 has fewer than 3840x2160.
inline bool FitsRenderTargets(Size size) {
	constexpr uint32_t tiles[4][2] {{256, 128}, {128, 128}, {128, 64}, {64, 64}};
	for (const auto& tile: tiles) {
		const auto count = [&](uint32_t width, uint32_t height) {
			return static_cast<uint64_t>((width + tile[0] - 1) / tile[0]) * ((height + tile[1] - 1) / tile[1]);
		};
		if (count(size.width, size.height) > count(OutputWidth, OutputHeight)) return false;
	}
	return true;
}
// The highest render height (a multiple of 8) whose size at `aspect` fits (FitsRenderTargets): 2160 at 16:9, 1832 at
// 21:9 (4344x1832), 1512 at 32:9 (5376x1512).
inline uint32_t MaxRenderHeight(double aspect) {
	for (uint32_t height = OutputHeight; height >= 720; height -= 8) {
		if (FitsRenderTargets(RenderSizeFor(height, aspect))) return height;
	}
	return 720;
}

// KYTY_RENDER_ASPECT's value: "<w>:<h>" or a number; 21:9 is the displays' 64:27. 0 when unset, at most 16:9 or wider
// than 32:9 (wider is untested: the game's targets were made for 3840 at most).
inline double ParseAspect(std::string_view text) {
	if (text == "21:9") return 64.0 / 27.0;
	double     w = 0.0, h = 1.0;
	const auto colon  = text.find(':');
	const auto number = [](std::string_view part) {
		double value = 0.0, scale = 0.0;
		for (const char c: part) {
			if (c == '.' && scale == 0.0) {
				scale = 1.0;
			} else if (c >= '0' && c <= '9') {
				if (scale == 0.0) {
					value = value * 10.0 + (c - '0');
				} else {
					scale /= 10.0;
					value += (c - '0') * scale;
				}
			} else {
				return -1.0;
			}
		}
		return part.empty() ? -1.0 : value;
	};
	if (colon == std::string_view::npos) {
		w = number(text);
	} else {
		w = number(text.substr(0, colon));
		h = number(text.substr(colon + 1));
	}
	if (w <= 0.0 || h <= 0.0) return 0.0;
	const double aspect = w / h;
	return aspect > GameAspect + 0.01 && aspect <= WidestAspect + 0.01 ? aspect : 0.0;
}
} // namespace DemonsSoulsRenderSize
} // namespace Loader
