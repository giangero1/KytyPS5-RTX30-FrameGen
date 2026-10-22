#include "loader/demonsSoulsHdOutput.h"
#include "loader/demonsSoulsRenderSize.h"

#include "graphics/host_gpu/renderer/demonsSouls.h"
#include "loader/guestCode.h"
#include "loader/runtimeLinker.h"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

namespace Loader::DemonsSoulsHdOutput {
namespace {

bool                    g_active = false;
std::array<uint64_t, 3> g_modes {}; // the three choices' immediates

// A nop of `length` bytes (1-4) in place of a cmovne.
void Nop(uint64_t address, uint32_t length) {
	constexpr uint8_t nops[4][4] {{0x90}, {0x66, 0x90}, {0x0f, 0x1f, 0x00}, {0x0f, 0x1f, 0x40, 0x00}};
	std::memcpy(reinterpret_cast<void*>(address), nops[length - 1], length);
}

} // namespace

bool Requested() {
	const char* value = std::getenv("KYTY_HD_OUTPUT");
	if (value == nullptr || *value == 0) return false;
	if (std::strcmp(value, "auto") != 0) return std::strcmp(value, "0") != 0;
	// "auto": the HD output when the automatic render height (the window's) is at most 1080 lines, so that the game's
	// output is the window's 1920x1080 (loader/demonsSoulsRenderSize.h); never with the PS5 Pro path (KYTY_UPSCALER: its
	// output is 4K) or a wider aspect (KYTY_RENDER_ASPECT). Not the default: on an RX 9070 XT (RADV) the HD path's
	// picture was about 15 times darker than the 4K path's at Boletaria's start (mean luma 1.1 against 16), and no faster
	// (67.5 against 71 fps with the 3D at 1080 both ways).
	const char* upscaler = std::getenv("KYTY_UPSCALER");
	const char* aspect   = std::getenv("KYTY_RENDER_ASPECT");
	if ((upscaler != nullptr && *upscaler != 0 && std::strcmp(upscaler, "off") != 0) || (aspect != nullptr && *aspect != 0))
		return false;
	const auto height = DemonsSoulsRenderSize::AutoHeight(false);
	return height != 0 && height <= HdHeight;
}

bool Active() {
	return g_active;
}

void SetMode(int mode) {
	if (!g_active) return;
	const auto value = static_cast<uint32_t>(mode);
	for (const auto site: g_modes) std::memcpy(reinterpret_cast<void*>(site + 1), &value, sizeof(value));
}

void Install(Program* program) {
	if (!Requested() || program == nullptr || !Libs::Graphics::DemonsSouls::IsSupportedGame() ||
	    program->file_name.filename() != "eboot.bin")
		return;
	const auto video     = GuestCode::FindUnique(*program, GuestCode::Pattern(VideoOutChoice));
	const auto renderer  = GuestCode::FindUnique(*program, GuestCode::Pattern(RendererChoice));
	const auto interface = GuestCode::FindUnique(*program, GuestCode::Pattern(InterfaceChoice));
	const auto buffers   = GuestCode::Find(*program, GuestCode::Pattern(DisplayBuffer), 3);
	const auto setter    = GuestCode::Find(*program, GuestCode::Pattern(DemonsSoulsRenderSize::SetterFixedPair), 2);
	const auto builder   = GuestCode::Find(*program, GuestCode::Pattern(DemonsSoulsRenderSize::ConstructorFixedPair), 2);
	const auto query     = GuestCode::Find(*program, GuestCode::Pattern(DemonsSoulsRenderSize::QueryFixedPair), 2);
	if (!video || !renderer || !interface || buffers.size() != 2 || setter.size() != 1 || builder.size() != 1 ||
	    query.size() != 1) {
		std::printf("Demon's Souls HD output: the display mode settings of a known build (01.007.000) not found; the 4K "
		            "output kept\n");
		return;
	}
	// Installation occurs before module initializers or guest threads run; program memory is writable to the host.
	g_modes = {*video, *renderer, *interface};
	for (const auto at: VideoOutOverrides) Nop(*video + at, 3);
	for (const auto at: RendererOverrides) Nop(*renderer + at, 3);
	Nop(*interface + InterfaceOverrides[0], 3);
	Nop(*interface + InterfaceOverrides[1], 4);
	for (const auto site: buffers) {
		std::memcpy(reinterpret_cast<void*>(site + 1), &HdWidth, sizeof(HdWidth));
		std::memcpy(reinterpret_cast<void*>(site + 6), &HdHeight, sizeof(HdHeight));
	}
	// The ceilings' mov r64, 2160 << 32 | 3840 (the immediate after the opcode's two bytes).
	const uint64_t ceiling = static_cast<uint64_t>(HdHeight) << 32u | HdWidth;
	std::memcpy(reinterpret_cast<void*>(setter[0] + 6), &ceiling, sizeof(ceiling));
	std::memcpy(reinterpret_cast<void*>(builder[0] + 9), &ceiling, sizeof(ceiling));
	std::memcpy(reinterpret_cast<void*>(query[0] + 5), &ceiling, sizeof(ceiling));
	// The interface's textures are 4K's in every mode, and an image sized from its texture divides it by the screen's
	// pixels per layout unit (2 at 4K, 1 at 1080p): at 1080p the HUD's images and frames, the menus' and the health
	// bars' came twice as large (the user 10-20). The divisor kept at 2.0: mov eax, 0x40000000; vmovd xmm0, eax.
	constexpr uint8_t divisor[16] {0xb8, 0x00, 0x00, 0x00, 0x40, 0xc5, 0xf9, 0x6e,
	                               0xc0, 0x0f, 0x1f, 0x80, 0x00, 0x00, 0x00, 0x00};
	if (const auto image = GuestCode::FindUnique(*program, GuestCode::Pattern(ImageScale))) {
		Nop(*image, 4);
		std::memcpy(reinterpret_cast<void*>(*image + 9), divisor, sizeof(divisor));
		std::printf("Demon's Souls HD output: the interface's images sized at the 4K scale\n");
	} else {
		std::printf("Demon's Souls HD output: the interface's image sizing not found; its images come twice as large\n");
	}
	// Another kind of image divides the same way: at 1080p the souls counter's backdrop, a gradient strip from the
	// icon to the number at 4K, came as a small black square. The same 9 bytes, without the nop7.
	if (const auto backdrop = GuestCode::FindUnique(*program, GuestCode::Pattern(BackdropScale))) {
		std::memcpy(reinterpret_cast<void*>(*backdrop + BackdropDivisor), divisor, 9);
		std::printf("Demon's Souls HD output: the souls counter's backdrop sized at the 4K scale\n");
	} else {
		std::printf("Demon's Souls HD output: the souls counter's backdrop sizing not found; it comes as a small square\n");
	}
	g_active = true;
	SetMode(2);
	std::printf("Demon's Souls HD output: the game's HD path (display mode 2: output, post-processing and interface at "
	            "1920x1080) is on\n");
}

} // namespace Loader::DemonsSoulsHdOutput
