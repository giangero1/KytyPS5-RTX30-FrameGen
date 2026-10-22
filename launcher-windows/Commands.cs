using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;

namespace KytyLauncher {

// The command lines the buttons run: run-windows.ps1 and precompile-windows.ps1 in a console window of their own, as
// launcher.ps1 started them (Get-PlayCommand, Get-PrecompileCommand), so run.cmd and the launcher stay one path.
sealed class Commands {
	readonly LauncherContext context;

	public Commands(LauncherContext context) {
		this.context = context;
	}

	Settings S { get { return context.Settings; } }

	// Windows PowerShell and cmd.exe by their paths: a PC whose PATH lacks their folder did not know "powershell"
	// (issue #55), and a file named "cmd" in the working folder made Windows ask which app should open it (issue #47).
	static string PowerShell() {
		var exe = Path.Combine(Environment.SystemDirectory, "WindowsPowerShell", "v1.0", "powershell.exe");
		return File.Exists(exe) ? "\"" + exe + "\"" : "powershell";
	}

	public static string Cmd() {
		var comspec = Environment.GetEnvironmentVariable("ComSpec");
		return !string.IsNullOrEmpty(comspec) && File.Exists(comspec) ? comspec : Path.Combine(Environment.SystemDirectory, "cmd.exe");
	}

	public bool Dlss { get { return context.FrameGenAvailable && S.Str("upscaler") == "dlss"; } }

	public string PlayCommand(GameInfo game = null) {
		var size = S.Str("resolution").Split('x');
		var arguments = new List<string> { "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", Quote(Path.Combine(context.Root, "run-windows.ps1")),
			"-Prompt", "-Follow", "-Width", size[0], "-Height", size.Length > 1 ? size[1] : "1080", "-Language", Number(S.Int("language")) };
		var ratio = S.Str("ratio");
		var wide = ratio == "21:9" || ratio == "32:9";
		// (-AspectFit with 21:9 or 32:9 would keep movies and menus at 16:9: the launcher's wider ratios fill the screen.)
		if (S.Bool("fullscreen")) {
			arguments.Add("-Fullscreen");
			if (S.Bool("aspect") && !wide) arguments.Add("-AspectFit");
		}
		if (!S.Bool("redzone")) arguments.Add("-NoRedZone");
		if (S.Bool("fps120")) arguments.Add("-Fps120");
		// Presents wait for the display's refresh (Vulkan FIFO): no tearing, and G-SYNC/FreeSync displays keep adapting.
		if (S.Bool("vsync")) arguments.AddRange(new[] { "-PresentMode", "Fifo" });
		if (S.Bool("lowlatency")) arguments.Add("-LowLatency");
		if (S.Bool("hdr")) arguments.Add("-Hdr");
		// The cheats file of this title, when it has one: the emulator applies the mods left enabled in it.
		if (S.Bool("cheats")) {
			var patch = (game ?? GameInfo.Read(S.Str("game"), context.PrecompileTool, context.Root)).CheatFile(context.Root);
			if (patch != null) arguments.AddRange(new[] { "-Patch", Quote(patch) });
		}
		var dlss = Dlss;
		// The upscaler's own render height (a chosen height is always passed: without one the game follows the window).
		var height = dlss ? S.Int("render_dlss") : S.Int("render");
		if (height > 0) arguments.AddRange(new[] { "-RenderHeight", Number(height) });
		if (wide) arguments.AddRange(new[] { "-Aspect", ratio });
		var quality = S.Str("quality").ToLowerInvariant();
		if (quality == "low" || quality == "lowest") arguments.AddRange(new[] { "-Quality", quality, "-TextureReduce", quality == "lowest" ? "2" : "1" });
		var fsrOn = context.FsrAvailable && S.Bool("fsr");
		if (fsrOn) arguments.Add("-Fsr");
		var frameGen = S.Int("framegen");
		if (!fsrOn && context.FrameGenAvailable && frameGen >= 2) arguments.AddRange(new[] { "-FrameGen", Number(frameGen - 1) });
		if (dlss) arguments.AddRange(new[] { "-Upscaler", "dlss" });
		// (DLSS makes the 4K output: the 1080p output only with the game's own upscale.)
		if (!dlss && S.Str("output") == "1080p") arguments.Add("-HdOutput");
		if (context.FrameGenAvailable && ((!fsrOn && frameGen >= 2) || dlss)) arguments.AddRange(new[] { "-StreamlineDir", Quote(context.Streamline) });
		var sets = new List<string>();
		// The emulator's panel at the top right of the game image: the frame rate and frame times, or with the GPU,
		// processor and memory too (F8 in the game switches it).
		var hud = S.Int("hud");
		if (hud >= 1) sets.Add("KYTY_FPS_HUD=" + Number(Math.Min(hud, 2)));
		// Safe GPU mode: Demon's Souls' compute work one dispatch after another (no GPU hangs, ~37% slower).
		if (S.Bool("safegpu")) sets.Add("KYTY_COMPUTE_CHAIN=0");
		if (sets.Count != 0) arguments.AddRange(new[] { "-Set", string.Join(",", sets) });
		var gamePath = S.Str("game");
		if (gamePath.Length != 0) arguments.AddRange(new[] { "-Game", Quote(gamePath) });
		// The console stays for the live log; after a crash it waits for a key.
		return PowerShell() + " " + string.Join(" ", arguments) + " & if !errorlevel! neq 0 pause";
	}

	public string PlayArguments(GameInfo game = null) {
		return "/v:on /s /c \"" + PlayCommand(game) + "\"";
	}

	public string PrecompileCommand(string option) {
		var arguments = new List<string> { "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", Quote(Path.Combine(context.Root, "precompile-windows.ps1")) };
		if (!string.IsNullOrEmpty(option)) arguments.Add(option);
		if (S.Bool("ecores") && context.EfficiencyMask != 0) arguments.AddRange(new[] { "-Affinity", context.EfficiencyMask.ToString(CultureInfo.InvariantCulture) });
		var gamePath = S.Str("game");
		if (gamePath.Length != 0) arguments.AddRange(new[] { "-Game", Quote(gamePath) });
		return PowerShell() + " " + string.Join(" ", arguments) + " & pause";
	}

	public string PrecompileArguments(string option) {
		return "/s /c \"" + PrecompileCommand(option) + "\"";
	}

	public void Start(string cmdArguments) {
		Process.Start(new ProcessStartInfo(Cmd(), cmdArguments) { UseShellExecute = true, WorkingDirectory = context.Root });
	}

	static string Quote(string text) {
		return "\"" + text + "\"";
	}

	static string Number(int value) {
		return value.ToString(CultureInfo.InvariantCulture);
	}
}

}
