// Runs the production loader patch against a private in-memory copy of a local decrypted ELF.
// No game data is distributed with this test or modified on disk.
#include "loader/demonsSoulsRenderSize.h"
#include "common/file.h"
#include "common/emulatorConfig.h"
#include "loader/elf.h"
#include "loader/guestCode.h"
#include "loader/runtimeLinker.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <vector>

namespace {
uint64_t begin_address = 0, end_address = 0;
uint32_t screen_width = 2560, screen_height = 1440;
void Check(bool ok) { if (!ok) std::abort(); }
uint32_t Word(uint64_t address) { uint32_t v; std::memcpy(&v, reinterpret_cast<void*>(address), 4); return v; }
}
// Isolate the loader from the renderer and guest execution; discovery/patching use production code.
namespace Libs::Graphics {
bool HostMemoryRangeIsReadable(uint64_t a, uint64_t n) { return a >= begin_address && a <= end_address && n <= end_address-a; }
void WindowOutputSize(uint32_t* width, uint32_t* height) { if (width) *width = screen_width; if (height) *height = screen_height; }
namespace DemonsSouls { bool IsSupportedGame() { return true; } }
}
namespace Loader {
Program::Program() = default;
Program::~Program() = default;
ThreadLocalStorage::~ThreadLocalStorage() = default;
namespace DemonsSoulsSuperResolution {
bool Active() { return false; }
bool HdOutput() { return false; }
bool SetLeastHeight(Program*, uint32_t) { return true; }
}
namespace DemonsSoulsHdOutput {
bool Active() { return false; }
void SetMode(int) {}
}
}

int main(int argc, char** argv) {
	using namespace Loader;
	using namespace DemonsSoulsRenderSize;
	if (argc != 2) { std::puts("Usage: demons_souls_output_integration_tests <decrypted eboot.bin>"); return 2; }
	Config::Initialize();
	for (int scenario = 0; scenario < 11; ++scenario) {
		Program program;
		program.file_name = "eboot.bin";
		program.elf = std::make_unique<Elf64>();
		program.elf->Open(argv[1]);
		Check(program.elf->IsValid());
		const auto* header = program.elf->GetEhdr();
		const auto* segments = program.elf->GetPhdr();
		uint64_t bytes = 0;
		for (unsigned i = 0; i < header->e_phnum; ++i)
			if (segments[i].p_type == PT_LOAD) bytes = std::max(bytes, segments[i].p_vaddr + segments[i].p_memsz);
		Check(bytes > 0 && bytes < 512ull * 1024 * 1024);
		std::vector<uint8_t> memory(bytes);
		begin_address = program.base_vaddr = reinterpret_cast<uint64_t>(memory.data());
		end_address = begin_address + bytes;
		for (unsigned i = 0; i < header->e_phnum; ++i)
			if (segments[i].p_type == PT_LOAD)
				program.elf->LoadSegment(begin_address + segments[i].p_vaddr, segments[i].p_offset, segments[i].p_filesz);
		const auto find_data = [&](const auto& values) {
			const auto* data = reinterpret_cast<const uint8_t*>(&values);
			const auto at = std::search(memory.begin(), memory.end(), data, data + sizeof(values));
			Check(at != memory.end());
			return reinterpret_cast<uint64_t>(&*at);
		};
		const auto context = GuestCode::FindUnique(program, GuestCode::Pattern(UpscalerContext));
		Check(context.has_value());
		const auto setter = GuestCode::FindUnique(program, GuestCode::Pattern(SetterFixedPair));
		const auto builder = GuestCode::FindUnique(program, GuestCode::Pattern(ConstructorFixedPair));
		const auto query = GuestCode::FindUnique(program, GuestCode::Pattern(QueryFixedPair));
		Check(setter && builder && query);
		const auto output_widths = find_data(OutputWidths), output_heights = find_data(OutputHeights);
		const auto render_heights = find_data(ModeHeights);
		const auto cinematic = find_data(CinematicSource);
		const auto canvas_floats = find_data(CanvasFloatPair);
		const auto float_widths = find_data(FloatWidths), float_heights = find_data(FloatHeights);
		screen_width = scenario == 1 ? 1920 : scenario == 2 ? 3840 : scenario == 3 ? 3440 : 2560;
		screen_height = scenario == 1 ? 1080 : scenario == 2 ? 2160 : scenario == 8 ? 720 : 1440;
		Config::ConfigOptions config;
		config.screen_width = screen_width;
		config.screen_height = screen_height;
		Config::Load(config);
		_putenv_s("KYTY_RENDER_HEIGHT", scenario == 4 ? "" : scenario == 5 ? "1512" : "1080");
		_putenv_s("KYTY_RENDER_ASPECT", scenario == 3 ? "21:9" : "");
		if (scenario == 6) *reinterpret_cast<uint8_t*>(*context) = 0; // Missing context signature.
		if (scenario == 7) *reinterpret_cast<uint32_t*>(output_heights) = 999; // Missing height table.
		if (scenario == 9) *reinterpret_cast<uint8_t*>(*builder) = 0; // Missing record constructor.
		if (scenario == 10) *reinterpret_cast<uint8_t*>(*query) = 0; // Missing size query.
		const auto before = memory;
		const bool installed = InstallSuperResolution(&program);
		if (scenario >= 6) {
			Check(!installed && memory == before); // No partial writes, including an unsupported output.
		} else {
			Check(installed);
			const auto output = OutputSizeFor(screen_width, screen_height, scenario == 3 ? ParseAspect("21:9") : GameAspect);
			Check(Word(*context + 1) == output.width && Word(*context + 6) == output.height);
			Check(Word(output_widths + TrinityMode * 4) == OutputWidth && Word(output_heights + TrinityMode * 4) == OutputHeight);
			for (const auto at : {*setter + 6, *builder + 9, *query + 5}) {
				Check(Word(at) == output.width && Word(at + 4) == output.height);
			}
			Check(Word(render_heights + TrinityMode * 4) == SuperResolutionRenderHeight(scenario >= 4 ? 1512 : 1080, output));
			Check(std::memcmp(reinterpret_cast<void*>(cinematic), CinematicSource, sizeof(CinematicSource)) == 0);
			Check(reinterpret_cast<const float*>(canvas_floats)[0] == CanvasFloatPair[0] &&
			      reinterpret_cast<const float*>(canvas_floats)[1] == CanvasFloatPair[1]);
			Check(reinterpret_cast<const float*>(float_widths)[1] == FloatWidths[1]);
			Check(reinterpret_cast<const float*>(float_heights)[1] == FloatHeights[1]);
		}
		std::printf("Output loader integration scenario %d: passed\n", scenario);
	}
	Config::Shutdown();
}
