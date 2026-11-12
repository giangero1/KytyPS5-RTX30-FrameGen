# v2026.11.06-upstream-merge — verified RTX 3070 build

This release packages the **exact locally built and regression-verified `1ad2a2aa` binaries**, with Clang-CL 19, ThinLTO, AVX2 (`x86-64-v3`), NVIDIA Streamline headers, and the official graphical launcher `KytyLauncher.exe`. `BUILD-INFO.txt` identifies the binary source and the final documentation/packaging commit separately. ZIP and per-file SHA256 checksums are included.

## Upstream fixes and credits

Thanks to **[chenxiao07/KytyPS5 v20261106](https://github.com/chenxiao07/KytyPS5/releases/tag/v20261106)** (`06cf66ce`) and its contributors. Merged in `1fb03a80`:

- **Native Stereo Sound Mixing & Headphone Speech Enhancement**: Redesigned 5.1/7.1 to stereo downmixing. Inter-channel correlation reduced from 0.74 to 0.27, speech and vocal intelligibility boosted by +7 dB, completely eliminating audio desynchronization and headphone crackle.
- **Scout Thread Ahead of Render Thread (+5–11% FPS)**: PM4 packet decoding and pipeline lookup run on a dedicated worker ahead of the main render thread, yielding 5–11% higher framerates in CPU-heavy Boletaria Castle scenes.
- **NVIDIA Empty Fragment Shader Stencil Pass Optimization**: Omits execution of redundant empty pixel shaders on NVIDIA hardware during stencil evaluation, saving ~0.57ms per frame near Archstones and dense fog gates.
- **8 GB VRAM Dynamic Budget Adaptation**: Automatically scales host memory reservation based on available GPU dedicated memory, preventing out-of-memory spikes on 8 GB cards during long play sessions.
- **Vulkan Swapchain F11 Destruction Fix**: Cleaned swapchain teardown when switching fullscreen with F11, preventing device-loss hangs.
- **01.005.000 Game Version Support**: Integrated `vmovddup` vector instruction emulation in guest code patcher for 01.005.000 update packages.
- **DLSS Sharpening & Automatic Texture LOD Bias**: Added RCAS DLSS sharpening controls (`KYTY_DLSS_SHARPNESS=0.3/0.6/0.9`) and automatic `log2(render / output)` negative texture LOD bias so mipmaps retain razor-sharp output resolution detail.

## Preserved RTX 30 integrations

**1920×1080 DLSS → 2560×1440 output + 2× frame generation**, original **3840×2160 UI/HUD canvas (`InterfaceScope`, `InterfaceTransformFor`)**, shared SR/FG frame tokens (`g_upscale_token`), and DXGI VRAM-pressure sampling/cache trimming are strictly retained.

**Automated validation:** Focused tests passed: `demons_souls_output_tests`, `demons_souls_output_integration_tests`, `page_manager_tests`, `save_data_backup_tests`, `command_scheduler_timeline_tests`, `host_image_allocation_tests`, `texture_cache_depth_readback_tests`, and all 4 `LauncherCommandTests.ps1` permutations. The binary pair is hash-identical to the verified build output.

**Installation:** extract the portable ZIP and use `KytyLauncher.exe` or `launcher.cmd`. Standard app/VC runtime DLLs and guides are included. **NVIDIA-signed Streamline and OpenDLSSG Enabler are separate prerequisites**; keep `version.dll` beside the actual emulator executable. Enable Windows HAGS and restart. Game files, saves, and personal settings are excluded.

See the [RTX 30 setup guide](https://github.com/giangero1/KytyPS5-RTX30-FrameGen/blob/feature/rtx3070-dlssg/docs/RTX30_FRAME_GEN_GUIDE.md) and [safe future-update playbook with a reusable request](https://github.com/giangero1/KytyPS5-RTX30-FrameGen/blob/feature/rtx3070-dlssg/docs/UPSTREAM_UPDATE_PLAYBOOK.md).

---

# v2026.10.27-upstream-merge — verified RTX 3070 build

This release packages the **exact locally built and regression-verified `4b3bdc27` binaries**, with Clang-CL 19, ThinLTO, AVX2 (`x86-64-v3`), NVIDIA Streamline headers, and the official graphical launcher `KytyLauncher.exe`. `BUILD-INFO.txt` identifies the binary source and the final documentation/packaging commit separately. ZIP and per-file SHA256 checksums are included.

## Upstream fixes and credits

Thanks to **[chenxiao07/KytyPS5 v20261027](https://github.com/chenxiao07/KytyPS5/releases/tag/v20261027)** (`06d69ef5`) and its contributors. Merged in `b08010e6`:

- **Lighter Shaders (EXEC Selects Feature 8)**: PS5 shader register writes masked lane-by-lane omit redundant unread lane writes, making translated shaders 8.5% smaller and compute shaders run 4–14% fewer instructions on NVIDIA. Existing precompiled shaders remain 100% valid.
- **VRR Displays (`-Set KYTY_VRR=1`)** (#221): On G-SYNC/FreeSync displays, frames flip immediately when completed rather than waiting for 60 Hz vblank intervals, eliminating micro-stutters below 60 FPS.
- **Frame Generation Camera-Cut Reset** (#174): Frame generation resets history cleanly across camera cuts (`inputs.cut` / `NoteCut()`), eliminating ghosted blended frames between scene transitions and dialogue shots.
- **VRAM Budget Accounting for DLSS-G & Super Resolution** (#215): DLSS frame generation and super resolution allocations are dynamically counted against the card's VRAM budget (`VideoMemoryReserve()`), preventing VRAM exhaustion during long play sessions.
- **120 FPS Menu Texture Retention** (#221): Texture collection intervals are measured in real time (10 s / 20 s) at any frame rate, eliminating world-texture eviction freezes after keeping menus open.
- **1440p 4K UI Layout & Ultrawide Display Buffer Fix** (#187, #198, #199, #202): Fullscreen menus and video backdrops retain correct aspect ratio on 1440p ultrawide setups without movie squeeze.
- **Memory Management & Return on 16 GB PCs** (#159, #174): Faster working set memory return to Windows after world transitions and refined guard page thresholds (1.5 GB to 2 GB) for seamless stability.

## Preserved RTX 30 integrations

**1920×1080 DLSS → 2560×1440 output + 2× frame generation**, original **3840×2160 UI/HUD canvas (`InterfaceScope`, `InterfaceTransformFor`)**, shared SR/FG frame tokens (`g_upscale_token`), and DXGI VRAM-pressure sampling/cache trimming are strictly retained.

**Automated validation:** Focused tests passed: `demons_souls_output_tests`, `demons_souls_output_integration_tests`, `page_manager_tests`, `save_data_backup_tests`, `command_scheduler_timeline_tests`, `host_image_allocation_tests`, `texture_cache_depth_readback_tests`, and all 4 `LauncherCommandTests.ps1` permutations. The binary pair is hash-identical to the verified build output.

**Installation:** extract the portable ZIP and use `KytyLauncher.exe` or `launcher.cmd`. Standard app/VC runtime DLLs and guides are included. **NVIDIA-signed Streamline and OpenDLSSG Enabler are separate prerequisites**; keep `version.dll` beside the actual emulator executable. Enable Windows HAGS and restart. Game files, saves, and personal settings are excluded.

See the [RTX 30 setup guide](https://github.com/giangero1/KytyPS5-RTX30-FrameGen/blob/feature/rtx3070-dlssg/docs/RTX30_FRAME_GEN_GUIDE.md) and [safe future-update playbook with a reusable request](https://github.com/giangero1/KytyPS5-RTX30-FrameGen/blob/feature/rtx3070-dlssg/docs/UPSTREAM_UPDATE_PLAYBOOK.md).

---

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

Your precompiled shaders stay valid: the shader translation is the same as in v20261109, except on AMD's older driver
for Radeon RX Vega and Polaris cards (the first item below).

- **RX Vega / Polaris with AMD's older driver: the black title screen** (#305, credit @FKF-GR): that driver ("AMD
  proprietary shader compiler" in the log's GPU line) refused every pipeline of 173 of the game's pixel shaders, the
  ones that read the GPU's barycentric coordinates without per-vertex values, the picture's post-processing among them.
  On that driver these shaders now get the same coordinates through AMD's own extension
  (VK_AMD_shader_explicit_vertex_parameter), which it lists. Other graphics cards translate them as before. Precompile
  again after updating on such a card. We could not run AMD's older driver here (the shaders pass validation and AMD's
  own compiler for Vega takes them), so please tell us in #305 whether the title shows.
- **New, experimental: parallel translation** (PR #293, credit @CarpetHead): with `-Set KYTY_SPECULATE=4` (run.cmd, or
  the launcher's Advanced > Extra options, "Command-line options"), worker threads translate the game's GPU work ahead
  of the render thread, which then only checks and commits it. Where the render thread limits the frame rate it is
  faster: +6% to +16% over the worlds in the author's test at 120 fps (Core i9-12900K, RTX 5090). It needs 28 GB of RAM
  or more (about 3.6 GB more than without it) and at least 9 logical CPUs on the performance cores; on other PCs it
  stays off and the log says why. Off by default; without it the emulator runs as before. Tested on Windows only.
- **A little faster**: the build's profile-guided optimization was trained again on this code. 1-1 at 120 fps on an
  RTX 5090: 70.3 -> 72.5 fps (the render thread 13.9 -> 13.4 ms a frame); Latria and the arena unchanged.
- **Thanks** to @CarpetHead for the pull request and the many rounds of measurements, and to @FKF-GR for the logs.

## New in v20261109

Your precompiled shaders stay valid: the shader translation is the same as in v20261108.

- **Long sessions on 8 GB graphics cards no longer slow down after many area loads, on 16 GB PCs too**: the game's
  page allocator frees blocks of memory but keeps them mapped and hands them to the next part of the game that asks,
  and the emulator kept the GPU buffers it had made over such a block for good. Over many world loads they grew from
  about 2 GB to over 3 GB, past an 8 GB card's budget, and Boletaria fell from about 60 to 48-52 fps. Now the buffers over a
  block the game frees go. In our runs on an 8 GB card with 16 GB of RAM (simulated; 42 world loads in 50 minutes) the
  buffers stayed at 1.5-1.9 GB and every world kept 60 fps; on an RTX 5090 they are about 0.4 GB smaller, at the same
  frame rate. What v20261108's notes said about 16 GB PCs (about 10% slower after 20 or more world changes) no longer
  applies. `-Set KYTY_ENGINE_FREE_BUFFERS=0` turns it off.
- **A pipeline the driver refuses gets a second try** (#305, credit @FKF-GR): an RX Vega 56 with AMD 26.5.2 refused some
  of the game's graphics pipelines. Since v20261108 their draws were left out, and at the title these included the
  picture's post-processing, so the screen stayed black. Now a refused pipeline is made again without the driver's
  optimizations before its draws are left out (a compute pipeline: before the game ends), and the log counts the
  outcomes at the end ("Pipelines the driver refused in play"). Also, on NVIDIA and on Linux with RADV, where pipelines
  are first made unoptimized for speed, a refused optimized build no longer replaces the pipeline that works.
- **An option against square holes in overlapping decals** (#315, credit @robbely, @zezoman): small square holes came
  and went in the Nexus candles' wax and at the bottom of some castle walls in 1-3. Those are decals that each read the
  scene's material data under them and write it back blended in. The game puts no wait between them: the PS5 runs them
  nearly in order, a PC graphics card runs overlapping ones at the same time, and pixels get lost in 2x2 blocks.
  `-Set KYTY_DRAW_READ_AFTER_WRITE=1` (launcher: Advanced > Extra options, "Command-line options") makes each such decal
  wait for the one before it, and the holes go. It is off by default because the waiting costs frame rate where there
  are hundreds of decals: at Boletaria's start without the 60 fps cap an RTX 5090 went from 73 to 61 fps (with the cap
  it held 60).
- **README** (#240, credit @blazouf): how to use a cheat file (`_Patches\PPSA01341.json`; `"enabled": false` leaves a
  cheat out; the file must be for the game's version), cheats on the Steam Deck (with a wrapper too), packing a `.zar`
  stops at paths over 260 characters (put the game folder near the drive root, or turn on Windows' long paths), and
  the black flicker on 16 GB PCs with little RAM left (#159, #319, credit @YC4R0): close the browser and other large
  programs; `-Set KYTY_HOT_READS_MIN_AVAIL_MB=0` keeps the flicker away, at the price of weaker blue-screen protection
  when the RAM runs completely full.
- **Long runs**: about 4.5 hours of world changes on this code (8 GB of video memory with and without 16 GB of RAM
  simulated, and the RTX 5090 pressing F11 at every stop): no memory leak, no hang, no crash.
- **Thanks** to @FKF-GR, @robbely, @zezoman, @blazouf and @YC4R0 for their reports and tests.

## 本版本的变化

已有的预编译着色器继续有效：着色器的翻译与 v20261109 相同，只有 Radeon RX Vega、Polaris 显卡使用 AMD 旧驱动时例外
（见第一条）。

- **RX Vega / Polaris 配 AMD 旧驱动：标题画面黑屏**（#305，感谢 @FKF-GR）：这个驱动（日志 GPU 一行写着“AMD
  proprietary shader compiler”）会拒绝游戏中 173 个像素着色器的全部管线，也就是读取 GPU 重心坐标、但不读逐顶点数据的
  那些，画面的后处理也在其中。现在在这个驱动上，这些着色器改用 AMD 自己的扩展（VK_AMD_shader_explicit_vertex_parameter，
  驱动有列出）取得同样的坐标，其他显卡的翻译不变。这类显卡更新后请重新预编译。我们这里没法运行 AMD 的旧驱动（这些着色器
  通过了校验，AMD 自己的 Vega 编译器也能编译），所以请在 #305 告诉我们标题画面是否正常显示。
- **新增实验功能：并行翻译**（PR #293，感谢 @CarpetHead）：加上 `-Set KYTY_SPECULATE=4`（run.cmd，或启动器的高级 >
  附加参数“命令行参数”）后，工作线程会提前翻译游戏的 GPU 工作，渲染线程只需检查并提交。在渲染线程限制帧率的地方会更快：
  作者在 120 fps 下逐个世界测试，提升 6% 到 16%（Core i9-12900K、RTX 5090）。需要 28 GB 以上内存（比不开多用约 3.6 GB），
  以及性能核上至少 9 个逻辑处理器；不满足的电脑上它不会开启，日志会说明原因。默认关闭，不开时模拟器和以前一样。目前只在 Windows 上测试过。
- **稍快一些**：重新训练了这个版本的 PGO（按运行情况优化）。RTX 5090 在 1-1、120 fps 下：70.3 -> 72.5 fps（渲染线程
  每帧 13.9 -> 13.4 ms）；Latria 和竞技场不变。
- **感谢** @CarpetHead 的 PR 和多轮测量，以及 @FKF-GR 提供的日志。

## v20261109 的变化

已有的预编译着色器继续有效：着色器的翻译与 v20261108 相同。

- **8 GB 显卡长时间游玩、多次读图后不再变慢（16 GB 内存的电脑也一样）**：游戏的页分配器释放内存块后并不解除映射，
  而是交给下一个申请内存的模块使用，模拟器在这种块上建立的 GPU 缓冲却一直保留。多次加载世界后，缓冲从约 2 GB 涨到
  3 GB 以上，超出 8 GB 显卡的预算，Boletaria 从 60 fps 掉到 48-52。现在游戏释放一个块时，块上的缓冲随之释放。在 8 GB
  显卡 + 16 GB 内存（模拟；50 分钟加载世界 42 次）的测试中，缓冲保持在 1.5-1.9 GB，每个世界都保持 60 fps；RTX 5090 上
  缓冲约少 0.4 GB，帧率不变。v20261108 说明里关于 16 GB 内存电脑“换图 20 次以上后帧率仍可能下降约 10%”的情况不再存在。
  `-Set KYTY_ENGINE_FREE_BUFFERS=0` 可以关闭。
- **驱动拒绝的管线会再试一次**（#305，感谢 @FKF-GR）：RX Vega 56（AMD 26.5.2）的驱动拒绝了游戏的部分图形管线。从
  v20261108 起这些管线的绘制会被跳过，而标题画面里被跳过的包括画面的后处理，所以一直黑屏。现在被拒绝的管线会先关掉
  驱动的优化重新创建一次，仍然失败才跳过它的绘制（计算管线：在结束游戏之前），日志结尾会统计结果（“Pipelines the
  driver refused in play”）。另外，在 NVIDIA 和 Linux 的 RADV 上（管线先以不优化的方式快速创建），驱动拒绝优化版本时
  不再用它替换掉已经能用的管线。
- **贴花重叠处的方块洞：新增一个选项**（#315，感谢 @robbely、@zezoman）：Nexus 蜡烛的蜡油和 1-3 部分城墙底部会出现
  时有时无的小方块洞。这些是贴花：每个贴花读出底下的场景材质数据，混合后再写回。游戏在贴花之间没有等待：PS5 基本按
  顺序执行，PC 显卡会同时执行重叠的贴花，像素就以 2x2 的块丢失。`-Set KYTY_DRAW_READ_AFTER_WRITE=1`（启动器：高级 >
  附加参数，“命令行参数”）让每个这样的贴花等前一个完成，方块洞就会消失。默认关闭，因为在有几百个贴花的地方，等待会
  降低帧率：Boletaria 起点不锁 60 帧时，RTX 5090 从 73 fps 降到 61（锁 60 帧时仍是 60）。
- **README**（#240，感谢 @blazouf）：金手指文件的用法（`_Patches\PPSA01341.json`；`"enabled": false` 的条目不生效；文件
  必须与游戏版本对应）、在 Steam Deck 上使用金手指（也可以配合包装程序）、打包 `.zar` 时路径超过 260 个字符会失败（把
  游戏文件夹放到靠近盘符根目录的位置，或开启 Windows 的长路径），以及 16 GB 内存的电脑剩余内存不足时的黑色闪烁
  （#159、#319，感谢 @YC4R0）：关闭浏览器等占内存的程序；`-Set KYTY_HOT_READS_MIN_AVAIL_MB=0` 可以消除闪烁，代价是内存
  完全用满时防蓝屏的保护变弱。
- **长时间测试**：在这版代码上进行了约 4.5 小时的连续换图（8 GB 显存，模拟 16 GB 内存和不限内存各一组，以及每个
  停留点都按 F11 的 RTX 5090）：没有内存泄漏，没有卡死，没有崩溃。
- **感谢** @FKF-GR、@robbely、@zezoman、@blazouf 和 @YC4R0 的反馈与测试。
