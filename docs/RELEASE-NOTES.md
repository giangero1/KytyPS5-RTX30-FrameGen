# v2026.10.22-upstream-merge — verified RTX 3070 build

This release packages the **exact locally built and gameplay-verified `f075b460` binaries**, with Clang-CL 19, ThinLTO, AVX2 (`x86-64-v3`), NVIDIA Streamline headers, and the official graphical launcher `KytyLauncher.exe`. `BUILD-INFO.txt` identifies the binary source and the final documentation/packaging commit separately. ZIP and per-file SHA256 checksums are included.

## Upstream fixes and credits

Thanks to **[chenxiao07/KytyPS5 v20261022](https://github.com/chenxiao07/KytyPS5/releases/tag/v20261022)** (`bfa6558f`) and its contributors. Merged in `598219f9`:

- **A new graphical launcher, `KytyLauncher.exe`:** full multi-page interface (Play, Display, Graphics, Game, Shaders, Advanced) in English or Chinese, with hardware, driver, and precompiled shader pre-flight diagnostics.
- **Low latency mode:** one finished frame waits for the screen instead of two, noticeably reducing input latency.
- **Dynamic render height:** follows display window when unset (e.g. 1080p rendering on 1080p display).
- **Shadow and texture fixes:** fixes surfaces past shortened sun-shadow distance looking dark, dark textures after level reload/death on 16 GB PCs, and ultrawide menu tearing.
- **AMD & Linux optimizations:** native float min/max instruction mapping (+8% GPU bound) and built-in FSR 3 upscaling/frame generation for Linux.

## Preserved RTX 30 integrations

**1920×1080 DLSS → 2560×1440 output + 2× frame generation**, original **3840×2160 UI/HUD canvas**, shared SR/FG frame tokens with distinct viewport IDs, and DXGI VRAM-pressure sampling/cache trimming are retained. The unified launcher selects the same executable for native and DLSS modes.

The user validated gameplay, DLSS, UI, saves and 2× FG with no observed regressions on RTX 3070, reporting **85–95+ displayed FPS in Boletaria** and smooth 100–120 FPS performance with driver 617.42 precompiled shaders. This is user-reported performance, not a controlled average or a guarantee. Existing reference screenshots show **43 base / 85 displayed FPS** for DLSS and **30 / 60** for native 1440p. FG does not raise the simulation rate to the displayed rate.

**Automated validation:** all seven focused tests pass: `demons_souls_output`, `launcher_commands`, `save_data_backup`, `page_manager`, `command_scheduler_timeline`, `host_image_allocation`, `texture_cache_depth_readback`. The binary pair is hash-identical to the verified build output. The broader historical BC3 atlas overlap failure and untested resolution/long-session cases remain documented in [validation limits](https://github.com/giangero1/KytyPS5-RTX30-FrameGen/blob/feature/rtx3070-dlssg/docs/DLSS_UI_CANVAS_VALIDATION.md); seven focused passes do not establish a fully passing suite.

**Installation:** extract the portable ZIP and use `KytyLauncher.exe` or `launcher.cmd`. Standard app/VC runtime DLLs, six screenshots and guides are included. **NVIDIA-signed Streamline and OpenDLSSG Enabler are separate prerequisites**; keep `version.dll` beside the actual emulator executable. Enable Windows HAGS and restart. Game files, saves and personal settings are excluded.

See the [RTX 30 setup guide](https://github.com/giangero1/KytyPS5-RTX30-FrameGen/blob/feature/rtx3070-dlssg/docs/RTX30_FRAME_GEN_GUIDE.md) and [safe future-update playbook with a reusable request](https://github.com/giangero1/KytyPS5-RTX30-FrameGen/blob/feature/rtx3070-dlssg/docs/UPSTREAM_UPDATE_PLAYBOOK.md).

---

# Historical notes: RTX 30 1080p DLSS → 1440p + 2× FG (experimental)

- Size the PS5 Pro SR/processing output to the launcher resolution while preserving the original 3840×2160 UI canvas. Menus, inventory and HUD keep their layout.
- Share the SR/FG frame token, retain separate viewport IDs, and log actual SR success/fallback counts. Valid SR output is retained when Streamline returns its budget warning.
- Add DXGI-first memory-pressure sampling, staged safe cache collection, completed-pool trimming and bounded resource diagnostics. Texture quality is unchanged; memory pressure remains possible on 8 GB cards.
- Resolve one launcher executable for both native and DLSS modes, with an explicit developer override.
- Include setup instructions and six user screenshots in the ZIP's `docs` folder. Bridge observations: **43 real / 85 displayed FPS** with 1080p DLSS versus **30 / 60** with native 1440p, both 2× FG. Views/builds were not controlled; no performance guarantee is implied.

**Required:** enable Windows hardware-accelerated GPU scheduling (HAGS), restart, and install the separate NVIDIA-signed Streamline runtime plus OpenDLSSG Enabler for RTX 30 FG. Runtime/proxy DLLs are not included. FG consumes GPU time and memory; displayed FPS is distinct from simulation/base FPS and input latency.

**Validation:** Release build, focused policy/GPU tests, 11 local v1.07 loader scenarios and capture metadata checks passed. The broader BC3 atlas overlap test fails at the same assertion with the existing branch texture cache. Ten-cycle inventory retention tests, matched-build benchmark averages and full runtime 1080p/4K/ultrawide coverage remain pending. Treat this as an experimental build.

Full guide and screenshots: https://github.com/giangero1/KytyPS5-RTX30-FrameGen/blob/fix/dlss-output-ui-canvas/docs/RTX30_FRAME_GEN_GUIDE.md

---

## New in this build

Your precompiled shaders stay valid: the shaders are the same as in v20261022. The release's hints now hold two
more whole playthroughs, so the launcher offers the precompile once more: only the new part is compiled (minutes).

- **FSR 3 upscaling for every graphics card:** in the launcher, Graphics > Upscaler > **FSR 3 (PS5 Pro path)**
  (`run.cmd -Upscaler fsr`). AMD's FSR 3 replaces Sony's PS5 Pro upscaler, as DLSS does: AMD, Intel and GeForce GTX
  cards get a real upscaler, built in, nothing to download. On an RTX 5090 at Boletaria's start, 1080 to 4K looked close
  to DLSS at the same height.
- **DLSS and FSR 3 for 1080p screens:** with Game output 1080p they make the 1080p picture directly (no 4K step), from
  render heights down to 360. Each render height in the launcher shows the mode it runs in (Quality, Balanced,
  Performance, Ultra Performance, DLAA); with the 4K output, 720 is DLSS's Ultra Performance. Graphics > **DLSS model**:
  Automatic, Transformer (preset K), or CNN (preset E, lighter on RTX 20 and 30 cards).
- **Game output "Automatic"** (the new default): the game's own 1080p output for a 16:9 game window (or screen, in
  full screen) at most 1080 lines high, 4K otherwise. On 1080p screens the picture is pixel for pixel and the graphics
  card saves the 4K post-processing (RTX 5090: 69.8 -> 78.3 fps; RX 9070 XT on Linux: +4.6%). Choose 4K to keep it
  (`run.cmd -Set KYTY_HD_OUTPUT=0`).
- **Every language in every release:** the game's language settings offer the text of all 23 languages (Chinese,
  Japanese and Korean too) and all 9 voices, whatever the release; no `param.json` edit (a contribution by @noir39,
  PR #139).
- **Sound:** no more crackling, and no delay that slowly grew in long sessions (#85).
- **Launcher:**
  - Experimental page: **Lock to 30 fps** (steady 30 at the normal game speed, for slower PCs), direct presentation
    for variable refresh rate displays (#37), **Keep memory resident** (a test against MEMORY_MANAGEMENT blue screens,
    #50), AMD Anti-Lag (#80).
  - Advanced: a **Command-line options** box (as after `run.cmd`), and a **Graphics card** choice on PCs with more than
    one (#132).
  - `KytyLauncher.exe --play` starts the game with the launcher's settings, without its window: for Steam's "Add a
    Non-Steam Game" with the Launch Options `--play` (#130).
  - Export shader hints writes a `.zip`, which GitHub takes as an attachment.
- **Fixes:**
  - With the 1080p output at render height 1080, the menus' 3D was black (the character in the equipment menu and in
    character creation) (#138).
  - PCs with two graphics cards ran the game on the one Windows' graphics settings did not choose, and the precompile
    on the other (#132). `run.cmd -Gpu 4090` (a number or part of the name) picks one.
  - A precompile that hung on an old graphics driver: a pipeline the driver never finishes is now left out after 5
    minutes, and the files such a run left behind no longer make every start offer the precompile (#126).
  - `kyty_emulator.exe` carries version information (Microsoft Defender's machine learning took the bare file for a
    trojan, #135).
- **Shader hints:** two whole playthroughs added, by @DevList1454 (#124) and @Mvii7 (#134): fewer first-time
  stutters in places our own recordings missed.
- **Linux:** the package is built with a Linux PGO profile (+3.4%, @the2ndday, PR #133); the render thread's locks and
  memory protection are cheaper (`KYTY_LINUX_FAST_LOCKS`, `KYTY_LINUX_COALESCE_MPROTECT`, on by default; +8% on an
  i5-14400, @the2ndday, PR #125); the texture offload for 16 GB PCs runs on Linux too (@Bios2000, PRs #136 and #137);
  FSR 3 upscaling is as sharp as it should be (@Bios2000, PR #131).
- **Thanks** to @noir39, @the2ndday, @Bios2000, @DevList1454, @Mvii7 and everyone who reported problems and tested.

## New in v20261022

![KytyLauncher.exe](https://raw.githubusercontent.com/chenxiao07/KytyPS5/v20261022/docs/images/launcher.png)

With an NVIDIA graphics card the shaders are the same as in v20261021b: your precompiled store stays valid. With an AMD
graphics card the launcher offers the precompile once, for the faster float min/max below (only the shaders it changes
are compiled).

- **A new launcher, `KytyLauncher.exe`:** double-click it (or `launcher.cmd`). The settings are on pages (Play,
  Display, Graphics, Game, Shaders, Advanced), each with a line on what it does, in English or Chinese (as Windows is,
  or chosen under Advanced). Before you start, the Play page checks the game and its version, the graphics card, the
  RAM, the precompiled shaders and DLSS/FSR. It keeps the old launcher's settings and starts the game the same way.
  Windows may ask before its first start ("More info", "Run anyway"): it is not signed. (The look follows a
  suggestion in issue #111.)
- **Low latency** in the launcher (off by default; `run.cmd -LowLatency`): one finished frame waits for the screen
  instead of about two, so what you press shows sooner (at 60 Hz about 15-23 ms instead of about 30); the frame pacing
  is a little less even.
- **The render height follows the window** when none is set (`run.cmd` without `-RenderHeight`): on a 1080p screen the
  3D renders at 1080 (faster), on larger screens at the game's 1440. A height chosen in the launcher or with
  `-RenderHeight` stays as chosen. (A contribution by @Bios2000, PR #121.)
- **Fixes:**
  - Low and Lowest graphics: surfaces past the shortened sun-shadow distance could look dark.
  - PCs with 16 GB of RAM: some textures could come back dark after a death or a level reload.
  - Ultrawide (21:9, 43:18): the tool belt menu and other menus drawn over the HUD were torn (reported by @intellq).
  The first two were found in the todojuegos67-bot fork, thanks.
- **AMD graphics cards:** float min/max as the GPU's own instructions, +8% where the graphics card limits the frame
  rate (measured on Linux; PR #114). AMD Anti-Lag can be tried with `run.cmd -LowLatency -Set KYTY_ANTI_LAG=1` (tested
  on Linux; PR #119).
- **Linux:**
  - AMD FSR 3 upscaling and frame generation are built in, for any graphics card: `KYTY_UPSCALER=fsr` and
    `KYTY_FRAMEGEN_BACKEND=fsr` (PRs #117 and #118).
  - Wider aspect ratios (`KYTY_RENDER_ASPECT`) ran at 2-3 fps: fixed (a contribution by @intellq, PR #123).
  - KDE Plasma on Wayland showed 1-2 frames a second: fixed (PR #113). On AMD (RADV), a pipeline seen for the first
    time stalls less (PR #115). Above 60 Hz the game keeps its speed without the Windows script's setting (PR #116).
  - The package has `precompile-linux.sh` (the shader precompile) and `get-dlss-linux.sh` (DLSS from NVIDIA's SDK,
    built on your PC) (a contribution by @Tutuzaooo, PR #110).
- **Thanks** to @Bios2000 (PRs #113 to #122), @Tutuzaooo, @intellq and the todojuegos67-bot fork for their
  contributions, and to everyone who reported problems and tested.

## 本版本的变化

已经预编译的着色器继续有效：着色器与 v20261022 相同。发布包的提示新增了两份完整通关的记录，所以启动器会再提示一次预编译：只编译新增的部分（几分钟）。

- **FSR 3 超分，任何显卡都可用：** 启动器「画质」>「超分辨率」>「FSR 3（PS5 Pro 路径）」（`run.cmd -Upscaler fsr`）。和 DLSS 一样，用 AMD 的 FSR 3 替代索尼的 PS5 Pro 超分：AMD、Intel 和 GeForce GTX 显卡也有了真正的超分，已内置，无需下载。在 RTX 5090 上的波雷塔利亚起点，从 1080 到 4K 的画面接近同高度的 DLSS。
- **1080p 屏幕的 DLSS 和 FSR 3：** 游戏输出选 1080p 时，它们直接生成 1080p 画面（不再经过 4K），渲染高度最低可到 360。启动器中每个渲染高度都标出对应的模式（质量、平衡、性能、超级性能、DLAA）；4K 输出时 720 是 DLSS 的超级性能模式。「画质」>「DLSS 模型」：自动、Transformer（预设 K）或 CNN（预设 E，在 RTX 20/30 系显卡上更轻）。
- **游戏输出「自动」**（新的默认）：16:9 且游戏窗口（全屏时为屏幕）高度不超过 1080 时使用游戏自己的 1080p 输出，否则 4K。1080p 屏幕上画面逐像素显示，显卡也省掉了 4K 后处理（RTX 5090：69.8 → 78.3 fps；Linux 上的 RX 9070 XT：+4.6%）。想保持 4K 请选 4K（`run.cmd -Set KYTY_HD_OUTPUT=0`）。
- **所有版本都能选所有语言：** 无论哪个区服版本，游戏的语言设置都提供全部 23 种文字（包括中文、日文、韩文）和全部 9 种语音，不再需要修改 `param.json`（@noir39 的贡献，PR #139）。
- **声音：** 不再有爆音，长时间游玩时声音逐渐延迟的问题也已修正（#85）。
- **启动器：**
  - 「实验性」页：**锁定 30 帧**（稳定 30 帧、游戏速度正常，适合较慢的电脑）、可变刷新率显示器的直接呈现（#37）、**保持内存常驻**（针对 MEMORY_MANAGEMENT 蓝屏的测试，#50）、AMD Anti-Lag（#80）。
  - 「高级」：**命令行参数**输入框（写法同 `run.cmd` 后面的参数），以及多显卡电脑上的**显卡**选择（#132）。
  - `KytyLauncher.exe --play` 用启动器的设置直接启动游戏、不显示启动器窗口：适合在 Steam「添加非 Steam 游戏」里使用，启动选项填 `--play`（#130）。
  - 导出着色器提示会生成 `.zip`，可以直接作为 GitHub 附件上传。
- **修正：**
  - 1080p 输出且渲染高度为 1080 时，菜单里的 3D 是黑的（装备菜单和捏人界面里的角色）（#138）。
  - 装有两块显卡的电脑上，游戏用的不是 Windows 图形设置选择的那块，预编译却用了另一块（#132）。可用 `run.cmd -Gpu 4090`（编号或名称的一部分）指定。
  - 旧显卡驱动导致预编译卡住：驱动始终编译不完的管线现在 5 分钟后跳过，这类运行留下的文件也不会再让每次启动都提示预编译（#126）。
  - `kyty_emulator.exe` 带上了版本信息（微软 Defender 的机器学习曾把没有版本信息的文件误判为木马，#135）。
- **着色器提示：** 加入了 @DevList1454（#124）和 @Mvii7（#134）的两份完整通关记录：在我们自己的录制没有覆盖到的地方，首次出现的卡顿更少。
- **Linux：** 发布包用 Linux 的 PGO 配置文件构建（+3.4%，@the2ndday，PR #133）；渲染线程的锁和内存保护更省（`KYTY_LINUX_FAST_LOCKS`、`KYTY_LINUX_COALESCE_MPROTECT`，默认开启；i5-14400 上 +8%，@the2ndday，PR #125）；面向 16 GB 电脑的纹理卸载在 Linux 上也启用了（@Bios2000，PR #136 和 #137）；FSR 3 超分恢复了应有的清晰度（@Bios2000，PR #131）。
- **感谢** @noir39、@the2ndday、@Bios2000、@DevList1454、@Mvii7 以及所有报告问题和帮忙测试的玩家。

## v20261022 的变化

NVIDIA 显卡：着色器与 v20261021b 相同，已经预编译的存储继续有效。AMD 显卡：启动器会提示预编译一次，用于下面更快的浮点 min/max（只编译受影响的着色器）。

- **全新启动器 `KytyLauncher.exe`：** 双击它（或 `launcher.cmd`）即可。设置分成几页（开始游戏、画面、画质、游戏、着色器、高级），每项下面都有一行说明，界面为英文或中文（跟随 Windows，也可以在「高级」里选择）。开始游戏页会在启动前检查游戏和版本、显卡、内存、预编译的着色器以及 DLSS/FSR。旧启动器的设置会保留，启动游戏的方式也完全相同。它没有数字签名，第一次运行时 Windows 可能会提示（点「更多信息」→「仍要运行」）。（界面风格参考了 issue #111 的建议。）
- **启动器中的「低延迟」**（默认关闭；`run.cmd -LowLatency`）：只让一帧等待显示（而不是约两帧），按键反馈更快（60 Hz 下约 15–23 ms，原来约 30 ms）；帧间隔会稍微不那么均匀。
- **未设置渲染高度时跟随窗口**（`run.cmd` 不带 `-RenderHeight`）：1080p 屏幕上 3D 以 1080 渲染（更快），更大的屏幕保持游戏的 1440。在启动器里或用 `-RenderHeight` 选择的高度保持不变。（@Bios2000 的贡献，PR #121。）
- **修正：**
  - 「低」和「最低」画质：超出缩短后日光阴影距离的表面可能显示发黑。
  - 16 GB 内存的电脑：死亡或重新载入关卡后，部分贴图可能变黑。
  - 超宽屏（21:9、43:18）：工具栏等压在 HUD 上的菜单会被撕开（@intellq 报告）。
  前两项是在 todojuegos67-bot 的 fork 中发现的，感谢。
- **AMD 显卡：** 浮点 min/max 改用 GPU 自己的指令，在显卡限制帧率时 +8%（在 Linux 上测得；PR #114）。可以用 `run.cmd -LowLatency -Set KYTY_ANTI_LAG=1` 试用 AMD Anti-Lag（在 Linux 上测试过；PR #119）。
- **Linux：**
  - 内置 AMD FSR 3 超分辨率和帧生成，任何显卡都可用：`KYTY_UPSCALER=fsr` 和 `KYTY_FRAMEGEN_BACKEND=fsr`（PR #117 和 #118）。
  - 更宽的画面比例（`KYTY_RENDER_ASPECT`）只有 2–3 fps 的问题已修正（@intellq 的贡献，PR #123）。
  - KDE Plasma（Wayland）上每秒只显示 1–2 帧的问题已修正（PR #113）。AMD（RADV）上首次遇到的管线卡顿更少（PR #115）。高于 60 Hz 时不需要 Windows 脚本的设置，游戏速度也保持正常（PR #116）。
  - 发布包中加入了 `precompile-linux.sh`（着色器预编译）和 `get-dlss-linux.sh`（用 NVIDIA 的 SDK 在你的电脑上构建 DLSS）（@Tutuzaooo 的贡献，PR #110）。
- **感谢** @Bios2000（PR #113 至 #122）、@Tutuzaooo、@intellq 以及 todojuegos67-bot 的 fork 的贡献，也感谢所有报告问题和帮忙测试的玩家。
