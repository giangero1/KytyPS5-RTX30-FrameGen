#pragma once

#include <sl_result.h>

namespace Libs::Graphics::FrameGen {
// Streamline checks the VRAM budget after a successful begin/endEvaluate. Its
// budget warning therefore still leaves a valid DLSS output; only errors fall back.
inline bool EvaluationSucceeded(sl::Result result) {
	return result == sl::Result::eOk || result == sl::Result::eWarnOutOfVRAM;
}
} // namespace Libs::Graphics::FrameGen
