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

## New in v20261021b

A hotfix for v20261021's precompile. The shaders are the same as in v20261021: your precompiled store stays valid.

- **A precompile that never finished (issue #107):** on PCs where the game had been played before, v20261021's
  precompile split its shards without end, and its "waiting" count kept growing. It loaded the game's own pipeline
  cache (`_PipelineCache\local`), whose data holds the compression dictionary NVIDIA's driver trained in play, and with
  that no pipeline could be kept. The precompile no longer loads that cache, and its shards are smaller (about 100
  pipelines each, at most 160). If you stopped a v20261021 precompile, run it again: what it finished is kept.
- **1080p output in the launcher:** "Game output" picks 4K (as on the PS5) or 1080p, the game's own HD mode, also
  `run.cmd -HdOutput`. In it the souls counter's backdrop is now right (a contribution by @intellq, PR #106), and FSR 3.1
  frame generation keeps the HUD sharp.
- **Cheats one by one:** the launcher's "Cheats" checkbox applies `_Patches\<title id>.json`, and "Select..." picks
  which of its cheats are on (a contribution by @Myoko, PR #105).
- **Linux:** source builds with clang can use profile-guided optimization, and the README explains how to make a
  profile (+7.5% on an i5-14400 with an RX 9060 XT; contributions by @the2ndday, PRs #108 and #109).

Earlier changes are on the release pages of v20261021 and before.

## 本版本的变化

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

## v20261021b 的变化

这是针对 v20261021 预编译问题的修复版本。着色器与 v20261021 相同：你已经预编译的存储继续有效。

- **预编译一直无法结束（issue #107）：** 在之前已经玩过游戏的电脑上，v20261021 的预编译会不停地拆分分片，"waiting" 的数量越来越多。原因是它读取了游戏自己的管线缓存（`_PipelineCache\local`），其中包含 NVIDIA 驱动在游玩时训练出的压缩字典，结果一个管线都无法保存。现在预编译不再读取这个缓存，分片也更小了（每个约 100 个管线，最多 160 个）。如果你中断过 v20261021 的预编译，重新运行即可：已经完成的部分会保留。
- **启动器中的 1080p 输出：** "Game output" 可以选择 4K（与 PS5 相同）或 1080p（游戏自带的高清模式），也可以用 `run.cmd -HdOutput`。这个模式下魂数旁边的深色条现在显示正常（@intellq 的贡献，PR #106），FSR 3.1 帧生成时 HUD 也保持清晰。
- **逐个选择作弊项：** 启动器的 "Cheats" 复选框会应用 `_Patches\<title id>.json`，"Select..." 可以选择启用其中哪些作弊项（@Myoko 的贡献，PR #105）。
- **Linux：** 用 clang 从源码构建时可以使用配置文件引导优化（PGO），README 说明了如何生成配置文件（i5-14400 + RX 9060 XT 上 +7.5%；@the2ndday 的贡献，PR #108 和 #109）。

更早的变化见 v20261021 及之前版本的发布页面。
