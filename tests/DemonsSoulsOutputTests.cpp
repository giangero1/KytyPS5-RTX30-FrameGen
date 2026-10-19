#include "loader/demonsSoulsRenderSize.h"
#include "loader/guestCode.h"
#include "local/video-memory-policy.h"
#ifdef KYTY_TEST_STREAMLINE
#include "local/frame-gen-result.h"
#endif

#include <array>
#include <cstdio>
#include <cstdlib>
#include <limits>

using namespace Loader::DemonsSoulsRenderSize;
static void Check(bool ok) { if (!ok) std::abort(); }
static bool Near(float x, float y) { return std::fabs(x - y) < 0.001f; }
static void SizeIs(Size size, uint32_t w, uint32_t h) { Check(size.width == w && size.height == h); }

int main() {
	using namespace Libs::Graphics;
	VideoMemoryPressure pressure;
	const auto sample = [](uint64_t room) { return VideoMemorySnapshot {true, VideoMemorySource::Dxgi, (8192ull << 20) - room, 8192ull << 20}; };
	VideoMemoryPressure boundary;
	boundary.Update(0, sample(512ull << 20)); Check(!boundary.active);
	boundary.Update(250, {}); Check(!boundary.active && !boundary.snapshot.valid);
	Check(pressure.Update(0, sample(511ull << 20)) && pressure.active);
	Check(!pressure.Update(249, sample(1024ull << 20)));
	Check(!pressure.Escalated(1999) && pressure.Escalated(2000));
	pressure.Update(2000, sample(769ull << 20));
	pressure.Update(3750, sample(769ull << 20)); Check(pressure.active);
	pressure.Update(4000, sample(769ull << 20)); Check(!pressure.active);
	pressure.Update(4250, sample(0)); Check(pressure.active);
	pressure.Update(4500, {}); Check(pressure.active && !pressure.Escalated(7000));
	pressure.Update(7000, sample(1024ull << 20)); Check(pressure.active);
	pressure.Update(7250, sample(768ull << 20)); Check(pressure.active && !pressure.recovering);
	pressure.Update(7500, sample(1024ull << 20));
	pressure.Update(9500, sample(1024ull << 20)); Check(!pressure.active);
	Check((VideoMemorySnapshot {true, VideoMemorySource::Vulkan, 9000, 8000}).Headroom() == 0);
	Check(PressureDownloadFits(32ull << 20, 64ull << 20, 32ull << 20));
	Check(!PressureDownloadFits(33ull << 20, 64ull << 20, 32ull << 20));
	Check(!PressureDownloadFits(32ull << 20, 16ull << 20, 0));
	Check(!PressureDownloadFits(1, 64ull << 20, 65ull << 20));
#ifdef KYTY_TEST_STREAMLINE
	using Libs::Graphics::FrameGen::EvaluationSucceeded;
	Check(EvaluationSucceeded(sl::Result::eOk));
	Check(EvaluationSucceeded(sl::Result::eWarnOutOfVRAM));
	Check(!EvaluationSucceeded(sl::Result::eErrorComputeFailed));
	Check(!EvaluationSucceeded(sl::Result::eErrorMissingResourceState));
#endif
	SizeIs(OutputSizeFor(2560, 1440), 2560, 1440);
	SizeIs(OutputSizeFor(1920, 1080), 1920, 1080);
	SizeIs(OutputSizeFor(3840, 2160), 3840, 2160);
	SizeIs(OutputSizeFor(7680, 4320), 3840, 2160);
	SizeIs(OutputSizeFor(3440, 1440), 2560, 1440);
	SizeIs(OutputSizeFor(3440, 1440, ParseAspect("21:9")), 3416, 1440);
	SizeIs(OutputSizeFor(5120, 1440, ParseAspect("32:9")), 5120, 1440);
	SizeIs(OutputSizeFor(1280, 720), 0, 0);
	SizeIs(OutputSizeFor(2560, 1440, std::numeric_limits<double>::quiet_NaN()), 0, 0);
	SizeIs(OutputSizeFor(0, 1440), 0, 0);
	const auto narrow = OutputSizeFor(1920, 1440);
	Check(narrow.width <= 1920 && narrow.height >= 1080);
	Check(SuperResolutionRenderHeight(1080, {2560, 1440}) == 1080);
	Check(SuperResolutionRenderHeight(900, {2560, 1440}) == 1080);
	Check(SuperResolutionRenderHeight(2160, {2560, 1440}) == 1440);
	// Omitted render setting uses the PS5 Pro default and still clamps/installs the output.
	Check(SuperResolutionRenderHeight(TrinityHeight, {2560, 1440}) == 1440);
	Check(SuperResolutionRenderHeight(TrinityHeight, {3840, 2160}) == TrinityHeight);
	Check(SuperResolutionRenderHeight(1080, {}) == 0);

    std::array<std::array<float, 2>, 6> native_movie {{{-1,1},{1,1},{-1,-1},{1,1},{-1,-1},{1,-1}}};
    Check(NativeFullscreenQuad(native_movie));
    auto authored_4k = native_movie;
    for (auto& xy: authored_4k) xy[0] = (xy[0] + 1) * 1.5f - 1;
    Check(!NativeFullscreenQuad(authored_4k));
    auto icon = native_movie;
    for (auto& xy: icon) { xy[0] *= 0.1f; xy[1] *= 0.1f; }
    Check(!NativeFullscreenQuad(icon));
	const auto scaled = InterfaceTransformFor({2560, 1440});
	Check(Near(3840 * scaled.xscale, 2560) && Near(2160 * scaled.yscale, 1440));
	const auto identity = InterfaceTransformFor({3840, 2160});
	Check(Near(identity.xscale, 1) && Near(identity.yscale, 1) && Near(identity.xshift, 0));
    // Logo/movie quads remain in the fixed 4K layout even with a 1440p viewport:
    // their logical centre (1920,1080) must land at the output's centre (1280,720).
    Check(Near(1920 * scaled.xscale, 1280) && Near(1080 * scaled.yscale, 720));
	for (const float side: {-1.0f, 0.0f, 1.0f}) {
		const auto t = InterfaceTransformFor({3416, 1440}, true, side);
		Check(Near(t.xscale, t.yscale));
		Check(Near(t.xshift, (3416 - 2560) * (side + 1) / 2));
		// A mask evaluated at the transformed window point must yield its original value.
		float row[4] {0.003f, -0.005f, 0.0f, 2.0f};
		const float original = row[0] * 1500 + row[1] * 1800 + row[3];
		t.InverseMask(row);
		Check(Near(original, row[0] * (1500 * t.xscale + t.xshift) + row[1] * (1800 * t.yscale) + row[3]));
	}

	// A complete transaction changes both SR/context dimensions while keeping the
	// independent 4K display/UI pair intact, including an unchanged render value.
	std::array<uint32_t, 8> image {3840, 2160, 3840, 2160, 3840, 2160, 2688, 1512};
	Loader::GuestCode::PatchBatch complete;
	complete.Value(reinterpret_cast<uint64_t>(&image[0]), uint32_t {2560});
	complete.Value(reinterpret_cast<uint64_t>(&image[1]), uint32_t {1440});
	complete.Value(reinterpret_cast<uint64_t>(&image[2]), uint32_t {2560});
	complete.Value(reinterpret_cast<uint64_t>(&image[3]), uint32_t {1440});
	complete.Value(reinterpret_cast<uint64_t>(&image[6]), image[6]);
	Check(complete.Apply());
	Check(image == std::array<uint32_t, 8> {2560, 1440, 2560, 1440, 3840, 2160, 2688, 1512});
	Loader::GuestCode::PatchBatch rejected;
	rejected.Value(reinterpret_cast<uint64_t>(&image[0]), uint32_t {1920});
	rejected.Value(reinterpret_cast<uint64_t>(&image[1]), uint32_t {1080});
	image[1] = 1600; // A validation mismatch at the last site must not leave the first site patched.
	Check(!rejected.Apply());
	Check(image[0] == 2560 && image[1] == 1600);
	std::puts("DemonsSoulsOutputTests: all cases passed");
}
