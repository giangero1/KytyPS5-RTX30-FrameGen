using System;
using System.IO;
using System.Text;
using System.Windows;

namespace KytyLauncher {

// Command line (for tests; a double-click passes none):
//   --dry-run <file>       the play, precompile and hints commands the current settings make, written to <file>
//   --screenshot <folder>  each page rendered to <folder>\<page>.png, then exit
//   --lang en|zh           the window's language for this run (the saved choice otherwise)
//   --no-art               (with --screenshot) no game art in the banner
//   --demo-path <text>     (with --screenshot) the game path box shows <text> (pictures for the release notes)
public partial class App : Application {
	protected override void OnStartup(StartupEventArgs e) {
		base.OnStartup(e);
		string dryRun = null, screenshots = null, lang = null;
		for (int i = 0; i + 1 < e.Args.Length; i++) {
			if (e.Args[i] == "--dry-run") dryRun = e.Args[++i];
			else if (e.Args[i] == "--screenshot") screenshots = e.Args[++i];
			else if (e.Args[i] == "--lang") lang = e.Args[++i];
			else if (e.Args[i] == "--demo-path") KytyLauncher.MainWindow.ScreenshotPath = e.Args[++i];
		}
		KytyLauncher.MainWindow.ScreenshotNoArt = Array.IndexOf(e.Args, "--no-art") >= 0;
		var context = new LauncherContext();
		context.Settings.ReadOnly = dryRun != null || screenshots != null;
		if (lang != null) Strings.Language = lang == "zh" ? UiLanguage.Chinese : UiLanguage.English;
		if (dryRun != null) {
			var text = new StringBuilder();
			var commands = new Commands(context);
			text.AppendLine("play:       cmd " + commands.PlayArguments());
			text.AppendLine("precompile: cmd " + commands.PrecompileArguments(""));
			text.AppendLine("hints:      cmd " + commands.PrecompileArguments("-ExportHints"));
			text.AppendLine(string.Format("efficiency cores: 0x{0:X}", context.EfficiencyMask));
			text.AppendLine("frame generation: " + (context.FrameGenAvailable ? "NVIDIA's Streamline runtime in " + context.Streamline
				: !context.RtxGpu ? "no NVIDIA RTX graphics card (DLSS off)" : "no NVIDIA-signed Streamline runtime (settings: '" + context.Settings.Str("streamline") + "')"));
			File.WriteAllText(dryRun, text.ToString(), new UTF8Encoding(false));
			Shutdown(0);
			return;
		}
		var window = new MainWindow(context);
		base.MainWindow = window;
		if (screenshots != null) {
			window.SaveScreenshots(screenshots);
			Shutdown(0);
			return;
		}
		window.Show();
	}
}

}
