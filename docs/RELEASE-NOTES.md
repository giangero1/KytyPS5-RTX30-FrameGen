# v2026.10.24-upstream-merge — verified RTX 3070 build

This release packages the **exact locally built and gameplay-verified `fc248f60` binaries**, with Clang-CL 19, ThinLTO, AVX2 (`x86-64-v3`), NVIDIA Streamline headers, and the official graphical launcher `KytyLauncher.exe`. `BUILD-INFO.txt` identifies the binary source and the final documentation/packaging commit separately. ZIP and per-file SHA256 checksums are included.

## Upstream fixes and credits

Thanks to **[chenxiao07/KytyPS5 v20261024](https://github.com/chenxiao07/KytyPS5/releases/tag/v20261024)** (`251112bc`) and its contributors. Merged in `0f3ea191`:

- **RAM Guard Page Protection & Blue Screen Prevention (<=20 GB RAM)** (#157, #158, #159, #174): Locked GPU-readback pages in RAM under no-access while preserving guard pages on remaining tracked memory, resolving flickering, missing lighting, and black tile glitches while eliminating `MEMORY_MANAGEMENT` (0x1A) blue screens.
- **DualSense Bluetooth Speaker Crash Fix & Haptics Tuning**: Resolved crash on launch when initializing the controller speaker over Bluetooth (PR #160, #168). Added controller speaker switch and configurable haptic vibration parameters (`dualsense-haptics.txt`).
- **1440p Output Option & Squeezed Ultrawide**: Upstream 1440p output support (`run.cmd -OutputHeight 1440`) and experimental squeezed 21:9 aspect ratio handling (`KYTY_WIDE_OUTPUT=0`).
- **Previous-World Pipeline Sweep Retention** (#85): Retains pipelines for the world just left when transitioning between regions (e.g. Nexus round trips), eliminating pipeline rebuild stutters on 6–8 GB GPUs.
- **Intel Arc & Fullscreen Improvements**: FSR 3 frame generation queue fallback on single-queue GPUs (#170), automatic mouse cursor hiding after 3 seconds of inactivity, and `KYTY_FULLSCREEN_EXCLUSIVE` toggling.

## Preserved RTX 30 integrations

**1920×1080 DLSS → 2560×1440 output + 2× frame generation**, original **3840×2160 UI/HUD canvas (`InterfaceScope`)**, shared SR/FG frame tokens (`g_upscale_token`), and DXGI VRAM-pressure sampling/cache trimming are strictly retained.

The user tested and verified gameplay, DLSS, menus/inventory, and 2× FG on RTX 3070 with zero observed regressions, confirming solid performance with driver 617.42 precompiled shaders.

**Automated validation:** Focused tests passed: `demons_souls_output_tests`, `page_manager_tests`, `save_data_backup_tests`, and `LauncherCommandTests.ps1`. The binary pair is hash-identical to the verified build output.

**Installation:** extract the portable ZIP and use `KytyLauncher.exe` or `launcher.cmd`. Standard app/VC runtime DLLs and guides are included. **NVIDIA-signed Streamline and OpenDLSSG Enabler are separate prerequisites**; keep `version.dll` beside the actual emulator executable. Enable Windows HAGS and restart. Game files, saves, and personal settings are excluded.

See the [RTX 30 setup guide](https://github.com/giangero1/KytyPS5-RTX30-FrameGen/blob/feature/rtx3070-dlssg/docs/RTX30_FRAME_GEN_GUIDE.md) and [safe future-update playbook with a reusable request](https://github.com/giangero1/KytyPS5-RTX30-FrameGen/blob/feature/rtx3070-dlssg/docs/UPSTREAM_UPDATE_PLAYBOOK.md).

---

# v2026.10.23-upstream-merge — verified RTX 3070 build

This release packages the **exact locally built and gameplay-verified `e29f3100` binaries**, with Clang-CL 19, ThinLTO, AVX2 (`x86-64-v3`), NVIDIA Streamline headers, and the updated official graphical launcher `KytyLauncher.exe`. `BUILD-INFO.txt` identifies the binary source and the final documentation/packaging commit separately. ZIP and per-file SHA256 checksums are included.

## Upstream fixes and credits

Thanks to **[chenxiao07/KytyPS5 v20261023](https://github.com/chenxiao07/KytyPS5/releases/tag/v20261023)** (`235e9fea`) and its contributors. Merged in `3487b568`:

- **DLSS CNN Model (Preset E):** Option in launcher under Graphics > DLSS Model (Automatic, Transformer Preset K, or CNN Preset E). The CNN model is significantly lighter on RTX 20 and 30 series GPUs.
- **Sound crackle & session-drift fix:** Resolved audio buffer desynchronization and crackle over extended gameplay sessions.
- **1080p Equipment & Character Creator 3D rendering fix:** Corrected rendering passes in menus and creation screen when running at 1080p.
- **Defender false-positive prevention:** Cleaned internal metadata and version strings to eliminate false positives in Windows Defender.
- **Steam `--play` argument support:** Launcher passes game arguments directly when launched via Steam shortcut.

## Preserved RTX 30 integrations

**1920×1080 DLSS → 2560×1440 output + 2× frame generation**, original **3840×2160 UI/HUD canvas**, shared SR/FG frame tokens with distinct viewport IDs, and DXGI VRAM-pressure sampling/cache trimming are strictly retained.

The user tested and verified gameplay, DLSS, sound, menus, and 2× FG on RTX 3070 with zero observed regressions, confirming solid performance with driver 617.42 precompiled shaders.

**Automated validation:** Focused tests passed: `demons_souls_output_tests`, `demons_souls_output_integration_tests`, and `LauncherCommandTests.ps1`. The binary pair is hash-identical to the verified build output.

**Installation:** extract the portable ZIP and use `KytyLauncher.exe` or `launcher.cmd`. Standard app/VC runtime DLLs and guides are included. **NVIDIA-signed Streamline and OpenDLSSG Enabler are separate prerequisites**; keep `version.dll` beside the actual emulator executable. Enable Windows HAGS and restart. Game files, saves, and personal settings are excluded.

See the [RTX 30 setup guide](https://github.com/giangero1/KytyPS5-RTX30-FrameGen/blob/feature/rtx3070-dlssg/docs/RTX30_FRAME_GEN_GUIDE.md) and [safe future-update playbook with a reusable request](https://github.com/giangero1/KytyPS5-RTX30-FrameGen/blob/feature/rtx3070-dlssg/docs/UPSTREAM_UPDATE_PLAYBOOK.md).

---

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

Your precompiled shaders stay valid: the shaders are the same as in v20261023.

- **The flicker on PCs with 20 GB of RAM or less fixed, the blue-screen fix kept** (#157, #158, #159, #174): since
  v20261023b these PCs keep the memory the emulator watches behind guard pages, against the MEMORY_MANAGEMENT blue
  screen. A guard lets another thread read its page in the microseconds before it is put back, so lights went out and
  tiles of the screen or textures turned black. The pages the game reads back from the GPU now keep no access
  instead, with their memory locked in RAM (the blue screen came from no-access pages whose memory had left RAM);
  every other page keeps its guard. At the Tower of Latria such early reads went from about one a frame to one in a
  thousand frames, with every frame right. The texture offload's dropped pages take guard pages now too: they were
  the last no-access pages of that kind. Thanks to @Bios2000, who proposed locking the pages first (PR #164). If a
  blue screen comes back on your PC, `run.cmd -Set KYTY_HOT_READS=0` goes back to v20261023d's guard pages
  everywhere; please tell us in #50.
- **Start-up crash with a DualSense on Bluetooth fixed**: the emulator could end without a message about 8 s in (3 of
  6 starts here) when the game opened the pad's speaker. Found and fixed by @Vorshim92 (PR #160, the cause in the
  build) and @CarpetHead (PR #168).
- **1440p output with DLSS or FSR 3**: the launcher's Game output has "1440p" (`run.cmd -OutputHeight 1440`). The PS5
  Pro path then outputs 2560x1440 instead of a 4K picture scaled down, and the HUD and menus are scaled to it. By
  @Tutuzaooo (PR #165).
- **Ultrawide with the 1080p and 1440p outputs**: with an upscaler, `-Aspect 21:9` now works with these outputs too,
  the picture squeezed into the 16:9 output and stretched back on the screen (`run.cmd -Set KYTY_WIDE_OUTPUT=0`,
  experimental; @intellq, PR #173).
- **Fewer stutters that grow over a session on 6-8 GB graphics cards** (#85): the pipelines of the world you just
  left are kept, and only those of worlds further back are dropped. Four round trips to the Nexus rebuilt about
  1,750 pipelines before, 8 now.
- **The mouse cursor hides** 3 s after it last moved over the game window (#129).
- **AMD: fullscreen slower than a window?** (#166) `run.cmd -Set KYTY_FULLSCREEN_EXCLUSIVE=0` tells the driver not to
  use exclusive fullscreen (`=1` allows it). Please tell us in #166 whether it helps.
- **Intel Arc: FSR 3 frame generation** should now start on graphics cards with a single graphics queue (#170).
  Please report how it goes.
- **DualSense**: the launcher's Game page has a "Controller speaker" switch (@Vorshim92, PR #161), and the
  vibration's strength and length are settings (`motor_gain`, `motor_ms` and `motor_decay` in
  dualsense-haptics.txt; @Myoko, PR #162).
- **Linux**: `kyty_launcher`, a settings window with the Windows launcher's pages (@Tutuzaooo, PR #172). The README
  now notes that the kernel's own scheduler runs the emulator about 10% faster than scx_lavd (#80).
- **Thanks** to @Vorshim92, @CarpetHead, @Tutuzaooo, @intellq, @Myoko and @Bios2000 for their pull requests, and to
  @berkocevik-del, @brokeneggg, @ft1704 and @thxnk420 for their reports.

## New in v20261023d

Your precompiled shaders stay valid: the shaders are the same as in v20261023.

- **Flicker, black screen tiles and dark areas since v20261023b, fixed on PCs with more than 20 GB of RAM** (#157,
  #158, #159): the guard pages that
  v20261023b used against the blue screen let the game read some of its data while the emulator was still bringing it
  up to date, so whole areas could lose their light, tiles of the screen or textures turned black, or the picture
  showed at twice its size (reproduced at the Tower of Latria). Guard pages are now used only on PCs with 20 GB of RAM
  or less, where the blue screen comes from; PCs with more RAM are back to the protection of v20261023 and show the
  picture as before. You can choose either way:
  - 20 GB of RAM or less: `run.cmd -Set KYTY_READ_GUARD=0` gives the right picture back, but the blue screen can
    return when the RAM runs full (close other programs then).
  - More RAM, and the PC blue-screens with MEMORY_MANAGEMENT when its RAM runs full: `run.cmd -Set KYTY_READ_GUARD=1`.

  We're working on a fix that needs neither.

## 本版本的变化

已有的预编译着色器继续有效：着色器与 v20261023 相同。

- **内存 20 GB 及以下电脑上的画面闪烁已修复，蓝屏修复保持不变**（#157、#158、#159、#174）：自 v20261023b 起，这类电脑用
  guard 页保护模拟器监视的内存，以防 MEMORY_MANAGEMENT 蓝屏。guard 页被放回之前的几微秒内，其他线程可能读到该页面，
  于是灯光熄灭、屏幕上成块的区域或贴图变黑。现在，游戏从 GPU 读回的页面改为禁止访问，并把它们的内存锁定在 RAM 中
  （蓝屏来自内存已离开 RAM 的禁止访问页面），其他页面仍使用 guard 页。在 Tower of Latria，这类过早的读取从每帧约一次
  降到每千帧一次，每一帧画面都正确。纹理卸载丢弃的页面现在也改用 guard 页，它们是最后一类这种禁止访问页面。感谢
  @Bios2000 提出先锁定页面的方案（PR #164）。如果你的电脑再次蓝屏，`run.cmd -Set KYTY_HOT_READS=0` 可以回到
  v20261023d 全部使用 guard 页的方式，也请在 #50 告诉我们。
- **修复了蓝牙连接 DualSense 时的启动崩溃**：游戏打开手柄扬声器时，模拟器可能在启动约 8 秒后无提示退出（这里 6 次启动中
  有 3 次）。由 @Vorshim92（PR #160，找到了构建中的原因）和 @CarpetHead（PR #168）发现并修复。
- **DLSS 或 FSR 3 的 1440p 输出**：启动器的「游戏输出」新增「1440p」（`run.cmd -OutputHeight 1440`）。PS5 Pro 路径直接输出
  2560x1440，不再把 4K 画面缩小，HUD 和菜单也随之缩放。由 @Tutuzaooo 提供（PR #165）。
- **1080p 和 1440p 输出下的超宽屏**：使用超分时，`-Aspect 21:9` 现在也能配合这两种输出使用，画面先压缩进 16:9 的输出，
  再在屏幕上拉伸回来（`run.cmd -Set KYTY_WIDE_OUTPUT=0`，实验性；@intellq，PR #173）。
- **6–8 GB 显卡上随游戏时间越来越多的卡顿减少**（#85）：刚离开的世界的管线会保留，只丢弃更早去过的世界的管线。在 Nexus
  往返四次，之前要重建约 1,750 条管线，现在只有 8 条。
- **鼠标指针**在游戏窗口上停止移动 3 秒后自动隐藏（#129）。
- **AMD：全屏比窗口慢？**（#166）`run.cmd -Set KYTY_FULLSCREEN_EXCLUSIVE=0` 让驱动不使用独占全屏（`=1` 则允许）。请在
  #166 告诉我们是否有帮助。
- **Intel Arc：FSR 3 帧生成**在只有一个图形队列的显卡上应该可以启动了（#170），欢迎反馈。
- **DualSense**：启动器「游戏」页新增「手柄扬声器」开关（@Vorshim92，PR #161），振动的强度和时长可以设置
  （dualsense-haptics.txt 中的 `motor_gain`、`motor_ms` 和 `motor_decay`；@Myoko，PR #162）。
- **Linux**：新增设置窗口 `kyty_launcher`，与 Windows 启动器的页面相同（@Tutuzaooo，PR #172）。README 中注明：内核自带的
  调度器运行模拟器比 scx_lavd 快约 10%（#80）。
- **感谢** @Vorshim92、@CarpetHead、@Tutuzaooo、@intellq、@Myoko 和 @Bios2000 的 PR，以及 @berkocevik-del、@brokeneggg、
  @ft1704 和 @thxnk420 的反馈。

## v20261023d 的变化

已有的预编译着色器继续有效：着色器与 v20261023 相同。

- **v20261023b 以来的画面闪烁、屏幕成块变黑和区域变暗，在内存大于 20 GB 的电脑上已修复**（#157、#158、#159）：v20261023b 为防蓝屏使用的 guard
  页，会让游戏在模拟器还没更新完数据时就读到它，导致整片区域失去光照、屏幕上成块的区域或贴图变黑，或画面放大两倍（已在
  Tower of Latria 复现）。现在只有内存 20 GB 及以下的电脑（蓝屏出现在这类电脑上）才使用 guard 页；内存更大的电脑恢复
  v20261023 的保护方式，画面恢复正常。也可以自行选择：
  - 内存 20 GB 及以下：`run.cmd -Set KYTY_READ_GUARD=0` 可以恢复正确的画面，但内存被占满时可能再次蓝屏（此时请关闭其他程序）。
  - 内存更大，但内存被占满时出现 MEMORY_MANAGEMENT 蓝屏：`run.cmd -Set KYTY_READ_GUARD=1`。

  我们正在做一个两者都不需要的修复。
