#pragma once
#include <cstdint>

namespace Libs::Graphics {
enum class VideoMemorySource : uint8_t { Unavailable, Dxgi, Vulkan };
struct VideoMemorySnapshot {
	bool                   valid  = false;
	VideoMemorySource      source = VideoMemorySource::Unavailable;
	uint64_t               usage = 0, budget = 0;
	[[nodiscard]] uint64_t Headroom() const { return budget > usage ? budget - usage : 0; }
};
// GPU-owned stale images can only be retired after preserving their bytes through
// the existing download path. Bound pressure-triggered downloads per collection.
inline bool PressureDownloadFits(uint64_t bytes, uint64_t utility_limit,
                                 uint64_t already_scheduled) {
	constexpr uint64_t limit = 64ull << 20;
	return bytes != 0 && bytes <= utility_limit && already_scheduled <= limit &&
	       bytes <= limit - already_scheduled;
}
// Monotonic milliseconds; a failed query is unknown, never evidence of recovery.
struct VideoMemoryPressure {
	bool                active = false, sampled = false, recovering = false;
	uint64_t            last_sample = 0, entered = 0, recovery_started = 0;
	VideoMemorySnapshot snapshot {};
	[[nodiscard]] bool  Due(uint64_t now) const { return !sampled || now - last_sample >= 250; }
	bool                Update(uint64_t now, VideoMemorySnapshot next) {
        if (!Due(now)) return false;
        sampled     = true;
        last_sample = now;
        snapshot    = next;
        if (!next.valid) {
            recovering = false;
            return true;
        }
        if (!active && next.Headroom() < (512ull << 20)) {
            active     = true;
            entered    = now;
            recovering = false;
        } else if (active) {
            if (next.Headroom() > (768ull << 20)) {
                if (!recovering) {
                    recovering       = true;
                    recovery_started = now;
                }
                if (now - recovery_started >= 2000) {
                    active     = false;
                    recovering = false;
                }
            } else
                recovering = false;
        }
        return true;
	}
	[[nodiscard]] bool Escalated(uint64_t now) const {
		return active && snapshot.valid && now - entered >= 2000;
	}
};
} // namespace Libs::Graphics
