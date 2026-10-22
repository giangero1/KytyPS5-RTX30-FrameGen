using System.Collections.Generic;
using System.Globalization;

namespace KytyLauncher {

enum UiLanguage { English, Chinese }

// The window's text in English and Simplified Chinese (the launcher's own language, not the console language the game
// is told). A key missing in Chinese shows the English text.
static class Strings {
	public static UiLanguage Language = UiLanguage.English;

	public static UiLanguage SystemLanguage() {
		return CultureInfo.CurrentUICulture.Name.StartsWith("zh", System.StringComparison.OrdinalIgnoreCase) ? UiLanguage.Chinese : UiLanguage.English;
	}

	public static string T(string key) {
		string text;
		if (Language == UiLanguage.Chinese && Chinese.TryGetValue(key, out text)) return text;
		return English.TryGetValue(key, out text) ? text : key;
	}

	public static string F(string key, params object[] values) {
		return string.Format(CultureInfo.CurrentCulture, T(key), values);
	}

	// The PS5's console languages in its order (run-windows.ps1 -Language takes the index), as the KytyPS5 launcher names them.
	public static readonly string[] ConsoleLanguagesEnglish = {
		"Japanese", "English (United States)", "French (France)", "Spanish (Spain)", "German", "Italian", "Dutch",
		"Portuguese (Portugal)", "Russian", "Korean", "Chinese (Traditional)", "Chinese (Simplified)", "Finnish", "Swedish",
		"Danish", "Norwegian", "Polish", "Portuguese (Brazil)", "English (United Kingdom)", "Turkish", "Spanish (Latin America)",
		"Arabic", "French (Canada)", "Czech", "Hungarian", "Greek", "Romanian", "Thai", "Vietnamese", "Indonesian" };
	static readonly string[] ConsoleLanguagesChinese = {
		"日语", "英语（美国）", "法语（法国）", "西班牙语（西班牙）", "德语", "意大利语", "荷兰语",
		"葡萄牙语（葡萄牙）", "俄语", "韩语", "中文（繁体）", "中文（简体）", "芬兰语", "瑞典语",
		"丹麦语", "挪威语", "波兰语", "葡萄牙语（巴西）", "英语（英国）", "土耳其语", "西班牙语（拉丁美洲）",
		"阿拉伯语", "法语（加拿大）", "捷克语", "匈牙利语", "希腊语", "罗马尼亚语", "泰语", "越南语", "印尼语" };
	public static string[] ConsoleLanguages { get { return Language == UiLanguage.Chinese ? ConsoleLanguagesChinese : ConsoleLanguagesEnglish; } }

	static readonly Dictionary<string, string> English = new Dictionary<string, string> {
		{"app.subtitle", "PS5 emulator for Windows"},
		{"nav.play", "Play"}, {"nav.display", "Display"}, {"nav.graphics", "Graphics"}, {"nav.game", "Game"},
		{"nav.shaders", "Shaders"}, {"nav.advanced", "Advanced"},
		{"nav.hint", "In the game: F8 switches the performance overlay, F9 saves a debug snapshot."},

		{"play.choose", "Choose your game"},
		{"play.checks", "Checks"}, {"play.gamebox", "Game"}, {"play.summary", "Current settings"},
		{"button.folder", "Folder…"}, {"button.zar", ".zar…"},
		{"button.play", "PLAY"}, {"button.precompile", "Precompile shaders"}, {"button.hints", "Export shader hints"},
		{"button.logs", "Logs folder"}, {"button.readme", "README"}, {"button.root", "Emulator folder"},
		{"button.streamline", "Streamline SDK folder…"}, {"button.cheats", "Choose…"},
		{"game.none", "No game chosen: choose the folder with eboot.bin and sce_sys (the game dump), or a .zar archive of it."},
		{"game.bad", "Not a game: no sce_sys\\param.json there (a .zar needs kyty_shader_precompile.exe next to the launcher)."},
		{"game.ok", "{0} · {1} · version {2}"},
		{"gpu.ok", "{0} · {1} GB video memory"}, {"gpu.okNoMemory", "{0}"}, {"gpu.none", "Graphics card not identified"},
		{"ram.ok", "{0} GB of RAM"},
		{"ram.low", "{0} GB of RAM: close other programs while playing, and keep the page file 'System managed'"},
		{"shaders.checking", "Precompiled shaders: checking…"},
		{"shaders.current", "Shaders precompiled for this graphics card"},
		{"shaders.outdated", "Shaders: precompile again (only what is new is compiled)"},
		{"shaders.none", "Shaders not precompiled yet: the game stutters the first time each effect appears"},
		{"shaders.unknown", "Precompiled shaders: not known (start the game once)"},
		{"dlss.ok", "DLSS ready (NVIDIA's Streamline found)"},
		{"dlss.noRtx", "DLSS needs an NVIDIA RTX graphics card"},
		{"dlss.noStreamline", "DLSS: choose NVIDIA's Streamline SDK folder (Graphics page)"},
		{"fsr.ok", "AMD FSR 3 frame generation ready"},
		{"fsr.none", "AMD FSR 3: amd_fidelityfx_vk.dll is not next to the emulator (README)"},

		{"summary.window", "{0} window"}, {"summary.fullscreen", "{0} fullscreen"}, {"summary.render", "3D {0}p"},
		{"summary.taa", "game's TAA"}, {"summary.dlss", "DLSS"}, {"summary.fg", "frame generation {0}"},
		{"summary.fsr", "FSR 3 frame generation"}, {"summary.hd", "1080p output"}, {"summary.hdr", "HDR"},
		{"summary.vsync", "VSync"}, {"summary.fps120", "up to 120 fps"},

		{"display.title", "Display"}, {"display.window", "Window"}, {"display.rate", "Frame rate"},
		{"resolution", "Resolution"}, {"resolution.desc", "The window's size; in fullscreen the screen mode."},
		{"ratio", "Aspect ratio"}, {"ratio.desc", "21:9 and 32:9 show more to the sides; the HUD keeps its shape, menus and movies are stretched."},
		{"ratio.169", "16:9 (PS5)"},
		{"fullscreen", "Fullscreen"},
		{"aspect", "Keep the aspect ratio (black bars)"}, {"aspect.desc", "In fullscreen on a screen of another shape."},
		{"hdr", "HDR"}, {"hdr.desc", "Turn on \"Use HDR\" in Windows display settings first; SDR otherwise."},
		{"fps120", "Up to 120 fps"}, {"fps120.desc", "The game's own frames above 60; movies stay at 60."},
		{"vsync", "VSync"}, {"vsync.desc", "No screen tearing; G-SYNC and FreeSync displays keep adapting."},
		{"lowlatency", "Low latency"},
		{"lowlatency.desc", "One finished frame waits for the screen instead of about two: at 60 Hz about 15-23 ms from the game's frame to the screen instead of about 30. A late frame is then no longer covered, so the pacing is a little less even."},

		{"graphics.title", "Graphics"}, {"graphics.resolution", "Resolution and upscaling"}, {"graphics.detail", "Detail"},
		{"graphics.fg", "Frame generation"},
		{"render", "3D render height"},
		{"render.desc", "Demon's Souls' 3D at {0}x{1}; higher is sharper and needs more of the graphics card."},
		{"render.capped", "Demon's Souls' 3D at {0}x{1} (1080 at most with the 1080p output)."},
		{"render.native", "native 4K"}, {"render.pro", "PS5 Pro"}, {"render.ps5", "PS5"},
		{"upscaler", "Upscaler"}, {"upscaler.game", "Game (PS5)"}, {"upscaler.dlss", "DLSS (PS5 Pro path)"},
		{"upscaler.ok", "DLSS in place of Sony's PS5 Pro upscaler: sharper than the game's stretch. 1440 = Quality, 1080 = Performance, 2160 = DLAA."},
		{"upscaler.noRtx", "DLSS needs an NVIDIA RTX graphics card: here the game's own TAA makes the picture."},
		{"upscaler.noStreamline", "DLSS needs NVIDIA's Streamline SDK: choose its folder under Frame generation."},
		{"output", "Game output"}, {"output.4k", "4K (PS5)"}, {"output.1080", "1080p"},
		{"output.desc", "1080p: the game's own HD mode, its effects and HUD at 1920x1080; less work for the graphics card; not with DLSS."},
		{"quality", "Graphics preset"}, {"quality.standard", "Standard (PS5)"}, {"quality.low", "Low"}, {"quality.lowest", "Lowest"},
		{"quality.desc", "Standard: the PS5's picture (below 12 GB of video memory large textures leave out their largest level). Low: shorter sun shadows, simpler distant detail, textures half the size per side. Lowest: shorter still, no ambient or torch shadows, textures a quarter the size. For PCs whose processor or graphics card limits the frame rate."},
		{"framegen", "DLSS frame generation"}, {"framegen.off", "Off"},
		{"framegen.ok", "NVIDIA RTX 40 or newer (3x and up: RTX 50). Shown frames = the game's frames x the multiplier; with VSync they are capped at the display's refresh (a 60 Hz display at 2x runs the game at 30 fps)."},
		{"framegen.noRtx", "Needs an NVIDIA RTX 40 or newer graphics card."},
		{"framegen.noStreamline", "Needs NVIDIA's Streamline SDK: download it, unpack it and choose its folder."},
		{"streamline", "Streamline SDK"},
		{"streamline.desc", "NVIDIA's Streamline SDK (a release zip from github.com/NVIDIA-RTX/Streamline/releases), unpacked: its folder or its bin\\x64. Only NVIDIA-signed files are used; nothing of NVIDIA's comes with the emulator."},
		{"streamline.none", "not chosen"},
		{"fsr", "AMD FSR 3 frame generation (2x, any graphics card; instead of DLSS)"},
		{"fsr.desc", "One generated frame between two of the game's, from its depth and motion. With VSync the shown frames are capped at the display's refresh."},
		{"fsr.missing", "Needs AMD's amd_fidelityfx_vk.dll next to the emulator (see the README)."},

		{"game.title", "Game"}, {"game.console", "Console"}, {"game.cheats", "Cheats"},
		{"language", "Console language"}, {"language.desc", "The language the game is told; it decides which text and voices it has."},
		{"cheats", "Cheats"}, {"cheats.desc", "_Patches\\<title id>.json (GoldHEN style): the emulator applies the cheats left on in it."},
		{"cheats.count", "{0} of {1} on"}, {"cheats.noFile", "No cheat file for this game"}, {"cheats.noGame", "Choose the game first"},

		{"shaders.title", "Shaders"}, {"shaders.precompile", "Precompile"}, {"shaders.share", "Share"},
		{"duration.minutes", "about {0} minutes"}, {"duration.hours", "about {0:0.0} hours"}, {"summary.options", "Options"},
		{"precompile.desc", "Compiles every shader of the game for this graphics card once: {0} on this PC, with the processor fully loaded. Closing its window stops it, and the next run continues. Needed again after a graphics driver update."},
		{"ecores", "Precompile on the efficiency cores only (slower, the PC stays responsive)"},
		{"ecores.none", "This processor has one kind of core."},
		{"hints.desc", "The shaders your play recorded, without the game's code, as my-hints-<game>.hints: attached to a GitHub issue, the next packages precompile them for everyone."},

		{"advanced.title", "Advanced"}, {"advanced.stability", "Overlay and stability"}, {"advanced.launcher", "Launcher"},
		{"hud", "Performance overlay"}, {"hud.off", "Off"}, {"hud.rate", "Frame rate"}, {"hud.detailed", "Detailed"},
		{"hud.desc", "Top right of the game image: the frame rate and frame times; Detailed adds the graphics card, processor and memory. F8 in the game switches it."},
		{"redzone", "Red-zone protection (recommended)"},
		{"redzone.desc", "Rewrites game code that keeps data just below its stack pointer, where Windows would overwrite it while handling the emulator's memory-tracking faults. Leave it on."},
		{"safegpu", "Safe GPU mode"},
		{"safegpu.desc", "Prevents the crashes that come from the graphics card hanging ('device lost', a black screen while the driver restarts, VIDEO_TDR_FAILURE): the game's compute work runs one step after another. About 37% lower frame rate. It does not help MEMORY_MANAGEMENT blue screens, which come from the RAM being full. Leave it off if the game runs without such crashes."},
		{"uilang", "Launcher language"}, {"uilang.auto", "Automatic"},
		{"credit", "Settings follow the KytyPS5 launcher (src/launcher) by the KytyPS5 developers."},

		{"dialog.game", "The game folder (eboot.bin, sce_sys)"},
		{"dialog.zar", "The game folder packed into a ZArchive"},
		{"dialog.streamline", "The unpacked NVIDIA Streamline SDK, or its bin\\x64 folder"},
		{"dialog.streamlineBad", "No sl.interposer.dll signed by NVIDIA in that folder or its bin\\x64.\nDownload the Streamline SDK from https://github.com/NVIDIA-RTX/Streamline/releases and choose the unpacked folder."},
		{"cheatsWindow", "Cheats - {0}"}, {"cheats.all", "Select all"}, {"cheats.none", "Select none"},
		{"ok", "OK"}, {"cancel", "Cancel"},
		{"cheats.unreadable", "The cheat file cannot be read:\n{0}\n\n{1}"}, {"cheats.empty", "That file holds no cheats:\n{0}"},
		{"cheats.changed", "The cheat file changed while the window was open; nothing was written."},
		{"cheats.countChanged", "The number of cheats does not match; nothing was written."},
		{"cheats.verifyFailed", "The write could not be verified; the backup was put back."},
		{"error.title", "KytyPS5 launcher"},
		{"error.start", "Could not start:\n{0}"},
	};

	static readonly Dictionary<string, string> Chinese = new Dictionary<string, string> {
		{"app.subtitle", "Windows 版 PS5 模拟器"},
		{"nav.play", "开始游戏"}, {"nav.display", "画面"}, {"nav.graphics", "画质"}, {"nav.game", "游戏"},
		{"nav.shaders", "着色器"}, {"nav.advanced", "高级"},
		{"nav.hint", "游戏中：F8 切换性能面板，F9 保存调试快照。"},

		{"play.choose", "选择游戏"},
		{"play.checks", "检查"}, {"play.gamebox", "游戏"}, {"play.summary", "当前设置"},
		{"button.folder", "文件夹…"}, {"button.zar", ".zar…"},
		{"button.play", "开始游戏"}, {"button.precompile", "预编译着色器"}, {"button.hints", "导出着色器提示"},
		{"button.logs", "日志文件夹"}, {"button.readme", "说明文档"}, {"button.root", "模拟器文件夹"},
		{"button.streamline", "Streamline SDK 文件夹…"}, {"button.cheats", "选择…"},
		{"game.none", "未选择游戏：请选择包含 eboot.bin 和 sce_sys 的游戏文件夹（游戏 dump），或其 .zar 压缩包。"},
		{"game.bad", "不是游戏：缺少 sce_sys\\param.json（使用 .zar 时启动器旁需要 kyty_shader_precompile.exe）。"},
		{"game.ok", "{0} · {1} · 版本 {2}"},
		{"gpu.ok", "{0} · 显存 {1} GB"}, {"gpu.okNoMemory", "{0}"}, {"gpu.none", "未识别到显卡"},
		{"ram.ok", "内存 {0} GB"},
		{"ram.low", "内存 {0} GB：游戏时请关闭其他程序，并将虚拟内存保持为「系统管理的大小」"},
		{"shaders.checking", "预编译着色器：检查中…"},
		{"shaders.current", "着色器已为本显卡预编译"},
		{"shaders.outdated", "着色器需要重新预编译（只编译新增部分）"},
		{"shaders.none", "着色器尚未预编译：每种效果第一次出现时会卡顿"},
		{"shaders.unknown", "预编译着色器：状态未知（请先启动一次游戏）"},
		{"dlss.ok", "DLSS 可用（已找到 NVIDIA Streamline）"},
		{"dlss.noRtx", "DLSS 需要 NVIDIA RTX 显卡"},
		{"dlss.noStreamline", "DLSS：请在「画质」页选择 NVIDIA Streamline SDK 文件夹"},
		{"fsr.ok", "AMD FSR 3 帧生成可用"},
		{"fsr.none", "AMD FSR 3：模拟器旁没有 amd_fidelityfx_vk.dll（见说明文档）"},

		{"summary.window", "窗口 {0}"}, {"summary.fullscreen", "全屏 {0}"}, {"summary.render", "3D {0}p"},
		{"summary.taa", "游戏 TAA"}, {"summary.dlss", "DLSS"}, {"summary.fg", "帧生成 {0}"},
		{"summary.fsr", "FSR 3 帧生成"}, {"summary.hd", "1080p 输出"}, {"summary.hdr", "HDR"},
		{"summary.vsync", "垂直同步"}, {"summary.fps120", "最高 120 帧"},

		{"display.title", "画面"}, {"display.window", "窗口"}, {"display.rate", "帧率"},
		{"resolution", "分辨率"}, {"resolution.desc", "窗口大小；全屏时为屏幕分辨率。"},
		{"ratio", "画面比例"}, {"ratio.desc", "21:9 与 32:9 能看到两侧更多画面；HUD 保持比例，菜单与过场动画会被拉伸。"},
		{"ratio.169", "16:9（PS5）"},
		{"fullscreen", "全屏"},
		{"aspect", "保持画面比例（黑边）"}, {"aspect.desc", "全屏且屏幕比例不同时生效。"},
		{"hdr", "HDR"}, {"hdr.desc", "需先在 Windows 显示设置中打开「使用 HDR」，否则为 SDR。"},
		{"fps120", "最高 120 帧"}, {"fps120.desc", "游戏本身以超过 60 帧运行；过场动画仍为 60 帧。"},
		{"vsync", "垂直同步"}, {"vsync.desc", "无画面撕裂；G-SYNC 与 FreeSync 显示器仍可自适应刷新。"},
		{"lowlatency", "低延迟"},
		{"lowlatency.desc", "只让一帧等待显示（而不是约两帧）：60 Hz 下从游戏出帧到显示约 15–23 ms（原约 30 ms）。迟到的帧不再有缓冲，帧间隔会稍不均匀。"},

		{"graphics.title", "画质"}, {"graphics.resolution", "分辨率与超分"}, {"graphics.detail", "细节"},
		{"graphics.fg", "帧生成"},
		{"render", "3D 渲染高度"},
		{"render.desc", "3D 画面为 {0}x{1}；越高越清晰，显卡负担越大。"},
		{"render.capped", "3D 画面为 {0}x{1}（1080p 输出时最高 1080）。"},
		{"render.native", "原生 4K"}, {"render.pro", "PS5 Pro"}, {"render.ps5", "PS5"},
		{"upscaler", "超分辨率"}, {"upscaler.game", "游戏自带（PS5）"}, {"upscaler.dlss", "DLSS（PS5 Pro 路径）"},
		{"upscaler.ok", "用 DLSS 替代索尼的 PS5 Pro 超分：比游戏自带的拉伸更清晰。1440 = 质量，1080 = 性能，2160 = DLAA。"},
		{"upscaler.noRtx", "DLSS 需要 NVIDIA RTX 显卡：当前由游戏自带的 TAA 生成画面。"},
		{"upscaler.noStreamline", "DLSS 需要 NVIDIA Streamline SDK：请在下方「帧生成」中选择其文件夹。"},
		{"output", "游戏输出"}, {"output.4k", "4K（PS5）"}, {"output.1080", "1080p"},
		{"output.desc", "1080p：游戏自带的 HD 模式，特效与 HUD 为 1920x1080；显卡负担更低；不能与 DLSS 同时使用。"},
		{"quality", "画质预设"}, {"quality.standard", "标准（PS5）"}, {"quality.low", "低"}, {"quality.lowest", "最低"},
		{"quality.desc", "标准：PS5 的画面（显存低于 12 GB 时大贴图会去掉最大一级）。低：日光阴影更短，远景细节更简单，贴图边长减半。最低：阴影更短，无环境光遮蔽与火把阴影，贴图为四分之一。适合 CPU 或显卡限制帧率的电脑。"},
		{"framegen", "DLSS 帧生成"}, {"framegen.off", "关闭"},
		{"framegen.ok", "需 NVIDIA RTX 40 或更新（3 倍及以上需 RTX 50）。显示帧数 = 游戏帧数 × 倍数；开启垂直同步时不超过显示器刷新率（60 Hz 显示器用 2 倍时游戏为 30 帧）。"},
		{"framegen.noRtx", "需要 NVIDIA RTX 40 或更新的显卡。"},
		{"framegen.noStreamline", "需要 NVIDIA Streamline SDK：下载并解压后选择其文件夹。"},
		{"streamline", "Streamline SDK"},
		{"streamline.desc", "NVIDIA Streamline SDK（github.com/NVIDIA-RTX/Streamline/releases 的发布包），解压后选择其文件夹或 bin\\x64。只使用 NVIDIA 签名的文件；模拟器不附带任何 NVIDIA 文件。"},
		{"streamline.none", "未选择"},
		{"fsr", "AMD FSR 3 帧生成（2 倍，任意显卡；替代 DLSS）"},
		{"fsr.desc", "根据游戏的深度与运动，在两帧之间生成一帧。开启垂直同步时显示帧数不超过显示器刷新率。"},
		{"fsr.missing", "需要将 AMD 的 amd_fidelityfx_vk.dll 放到模拟器旁（见说明文档）。"},

		{"game.title", "游戏"}, {"game.console", "主机"}, {"game.cheats", "作弊"},
		{"language", "主机语言"}, {"language.desc", "告诉游戏的系统语言；游戏支持哪些文字与语音由游戏决定。"},
		{"cheats", "作弊"}, {"cheats.desc", "_Patches\\<title id>.json（GoldHEN 格式）：模拟器会应用其中开启的作弊项。"},
		{"cheats.count", "已开启 {0}/{1} 项"}, {"cheats.noFile", "本游戏没有作弊文件"}, {"cheats.noGame", "请先选择游戏"},

		{"shaders.title", "着色器"}, {"shaders.precompile", "预编译"}, {"shaders.share", "分享"},
		{"duration.minutes", "约 {0} 分钟"}, {"duration.hours", "约 {0:0.0} 小时"}, {"summary.options", "选项"},
		{"precompile.desc", "为本显卡把游戏的所有着色器编译一次：在这台电脑上{0}，期间 CPU 满载。关闭其窗口即可停止，下次会继续。更新显卡驱动后需要重新预编译。"},
		{"ecores", "仅在能效核上预编译（较慢，但电脑仍保持流畅）"},
		{"ecores.none", "此处理器只有一种核心。"},
		{"hints.desc", "把你游玩时记录的着色器（不含游戏代码）导出为 my-hints-<game>.hints：附在 GitHub issue 里，之后的版本会为所有人预编译它们。"},

		{"advanced.title", "高级"}, {"advanced.stability", "性能面板与稳定性"}, {"advanced.launcher", "启动器"},
		{"hud", "性能面板"}, {"hud.off", "关闭"}, {"hud.rate", "帧率"}, {"hud.detailed", "详细"},
		{"hud.desc", "显示在游戏画面右上角：帧率与帧时间；「详细」另含显卡、处理器与内存。游戏中按 F8 切换。"},
		{"redzone", "红区保护（推荐）"},
		{"redzone.desc", "改写把数据放在栈指针正下方的游戏代码：否则 Windows 处理模拟器内存追踪的异常时会覆盖这些数据。请保持开启。"},
		{"safegpu", "安全 GPU 模式"},
		{"safegpu.desc", "防止显卡挂起导致的崩溃（device lost、驱动重启时黑屏、VIDEO_TDR_FAILURE 蓝屏）：游戏的计算任务逐个执行。帧率约降低 37%。对内存占满引起的 MEMORY_MANAGEMENT 蓝屏无效。若没有此类崩溃请保持关闭。"},
		{"uilang", "启动器语言"}, {"uilang.auto", "自动"},
		{"credit", "设置项沿用 KytyPS5 开发者的 KytyPS5 启动器（src/launcher）。"},

		{"dialog.game", "游戏文件夹（eboot.bin、sce_sys）"},
		{"dialog.zar", "打包为 ZArchive 的游戏文件夹"},
		{"dialog.streamline", "解压后的 NVIDIA Streamline SDK 文件夹，或其 bin\\x64 文件夹"},
		{"dialog.streamlineBad", "该文件夹及其 bin\\x64 中没有 NVIDIA 签名的 sl.interposer.dll。\n请从 https://github.com/NVIDIA-RTX/Streamline/releases 下载 Streamline SDK，并选择解压后的文件夹。"},
		{"cheatsWindow", "作弊 - {0}"}, {"cheats.all", "全选"}, {"cheats.none", "全不选"},
		{"ok", "确定"}, {"cancel", "取消"},
		{"cheats.unreadable", "无法读取作弊文件：\n{0}\n\n{1}"}, {"cheats.empty", "该文件中没有作弊项：\n{0}"},
		{"cheats.changed", "窗口打开期间作弊文件被修改，未写入任何内容。"},
		{"cheats.countChanged", "作弊项数量不一致，未写入任何内容。"},
		{"cheats.verifyFailed", "写入后校验失败，已恢复备份。"},
		{"error.title", "KytyPS5 启动器"},
		{"error.start", "无法启动：\n{0}"},
	};
}

}
