# v2026.11.17-upstream-merge — verified RTX 3070 build

This release packages the **exact locally built and gameplay-verified `a21b6aac` binaries**, built with Clang-CL 19, ThinLTO, retrained PGO, AVX2 (`x86-64-v3`), NVIDIA Streamline headers (`KYTY_FRAMEGEN_STREAMLINE=1`), and the rebuilt graphical launcher `KytyLauncher.exe` (including PR #348's Malwarebytes false-positive fix). `BUILD-INFO.txt` identifies the binary source and the final documentation/packaging commit separately. ZIP and per-file SHA256 checksums are included.

**Shader cache compatibility:** Existing NVIDIA precompiled shaders from `v2026.11.10-upstream-merge` remain 100% valid — no shader recompilation is needed on NVIDIA GPUs.

## Upstream fixes and credits

Thanks to **[chenxiao07/KytyPS5 v20261117](https://github.com/chenxiao07/KytyPS5/releases/tag/v20261117)** (`ce6a27df`, spanning `v20261113`–`v20261117`, 151 commits) and its contributors. Merged cleanly in `a21b6aac`:

- **≤12 GB VRAM Texture Streaming Hitch & RAM Reduction (`v20261113`, #85, credit @brokeneggg)**: The game now streams large block-compressed textures without the top mip levels that ≤12 GB GPUs drop anyway, regardless of host RAM (`KYTY_TEXTURE_REDUCE_GAME=0` restores whole streaming). Upstream 1-hour world-change test (simulated 6 GB VRAM): slow frames `163 → 31`, worst frame `149 → 60 ms`, RAM growth `+384 → +21 MiB/hr`.
- **Long-Session 0.2–15 s Freeze Fix (`v20261116`)**: Retries guest memory copies interrupted by Windows page faults where Windows cleared upper AVX register halves, eliminating sudden ~100-pipeline recompilation freezes after 1–2.5 hours of play.
- **Smoother World Transitions & Faster Loads (`v20261116`)**:
  - **Flip-wait GPU request servicing (`KYTY_SERVE_IN_FLIP_WAITS`)**: GPU thread now services guest memory maps/unmaps/readbacks while waiting for frame flip (`16.2 → 0.9 ms` game-thread waits, world-unload 1% lows `25.6 → 37.2 fps`, world appearance `6.2 → 5.0 s`).
  - **Windows upload worker priority (`KYTY_UPLOAD_PRIORITY`)**: Eliminates 3–44 ms whole-emulator stalls during area loads (`22 pauses → 0` across 6 loads).
- **Malwarebytes Launcher False-Positive Fix (`v20261117`, #346 / PR #348, credit @DevList1454)**: `KytyLauncher.exe` now stores embedded keyboard prompt textures uncompressed (~3.3 MB exe) so Malwarebytes ML detection no longer flags it.
- **Precompiler Live Progress (`v20261116`, #334, credit @shark20619)**: Prints each stage (shader list, plans, shards, merge) with elapsed time and live shard progress.
- **Xbox Controller View Button → Toolbelt (`v20261113`–`v20261114`, #328 / #332, credit @kms810530-arch)**: View now sends a touch-pad press on the left half so Demon's Souls opens the Toolbelt reliably.
- **Version `01.005.000` King Allant Fog-Gate Freeze Fix (`v20261113`–`v20261114`, #327, credit @Lukeyrich)**: Loader guard now repairs all 10 pending resource request lists.
- **PS5 Floating-Point Environment (`v20261113`)**: Guest threads and fibers run with PS5 MXCSR (`FTZ + DAZ`, `-Set KYTY_GUEST_FP_ENV=0` to revert).
- **Retrained PGO & Parallel Translation (`KYTY_SPECULATE=4`) Overhaul (`v20261113`–`v20261117`, PRs #321, #329, #336–#339, #342, #345, credit @CarpetHead & @Bios2000)**: Retrained PGO profile plus major stability and area-crossing stutter reductions when testing opt-in `KYTY_SPECULATE=4`.

## Preserved RTX 30 integrations

**1920×1080 DLSS → 2560×1440 output + 2× frame generation**, original **3840×2160 UI/HUD canvas (`InterfaceScope`, `InterfaceTransformFor`)**, shared SR/FG frame tokens (`g_upscale_token`), 64 MiB VMA heap sizing, and DXGI VRAM-pressure sampling/cache trimming are strictly retained.

**Automated & gameplay validation:** Passed 8/8 focused CTests (`page_manager`, `demons_souls_output`, `launcher_commands`, `save_data_backup`, `command_scheduler_timeline`, `host_image_allocation`, `texture_cache_depth_readback`, `vulkan_recording`), 10/10 `demons_souls_output_integration_tests` scenarios, and 4/4 `LauncherCommandTests.ps1` permutations. Gameplay verified on RTX 3070 with OpenDLSSG 2× Frame Generation (`status=0x0 presented=2 max_generated=1`) and `KYTY_TEXTURE_REDUCE_GAME` active with zero crashes or errors.

**Installation:** extract the portable ZIP and use `KytyLauncher.exe` or `launcher.cmd`. Standard app/VC runtime DLLs and guides are included. **NVIDIA-signed Streamline and OpenDLSSG Enabler are separate prerequisites**; keep `version.dll` beside the actual emulator executable. Enable Windows HAGS and restart. Game files, saves, and personal settings are excluded.

See the [RTX 30 setup guide](https://github.com/giangero1/KytyPS5-RTX30-FrameGen/blob/feature/rtx3070-dlssg/docs/RTX30_FRAME_GEN_GUIDE.md) and [safe future-update playbook with a reusable request](https://github.com/giangero1/KytyPS5-RTX30-FrameGen/blob/feature/rtx3070-dlssg/docs/UPSTREAM_UPDATE_PLAYBOOK.md).

---

# v2026.11.10-upstream-merge — verified RTX 3070 build

This release packages the **exact locally built and regression-verified `9e4ff1d3` binaries**, with Clang-CL 19, ThinLTO, AVX2 (`x86-64-v3`), NVIDIA Streamline headers, and the official graphical launcher `KytyLauncher.exe`. `BUILD-INFO.txt` identifies the binary source and the final documentation/packaging commit separately. ZIP and per-file SHA256 checksums are included.

## Upstream fixes and credits

Thanks to **[chenxiao07/KytyPS5 v20261110](https://github.com/chenxiao07/KytyPS5/releases/tag/v20261110)** (`3ec8cf56`) and its contributors. Merged in `65b16fbe`:

- **8 GB VRAM Memory Leak & Slowdown Fix (`KYTY_ENGINE_FREE_BUFFERS`)**: The game's page allocator frees blocks without unmapping them; the emulator now immediately purges GPU buffers over freed blocks. Resolves performance drop-offs on 8 GB cards (e.g. RTX 3070) where multi-zone world changes caused memory to grow from 2 GB to >3 GB and degraded Boletaria from 60+ down to 48–52 FPS. VRAM stays rock-solid at 1.5–1.9 GB across 40+ world loads.
- **Parallel Speculative Translation (`KYTY_SPECULATE=4`, PR #293 by @CarpetHead)**: Worker threads translate GPU packets ahead of the render thread, which then only checks and commits. Yields a +6% to +16% FPS uplift in render-thread-bound scenes on CPUs with ≥9 P-core threads and 28 GB+ RAM.
- **Physics Crash Fix**: Eliminated the `eboot+0x1ab6047` crash when physics jobs spawned without an active object.
- **Decal Overlap Square Artifact Fix (`KYTY_DRAW_READ_AFTER_WRITE=1`)**: Resolves small black square pixel artifacts appearing where decals overlap in Nexus candle wax and 1-3 castle wall textures.
- **Retrained PGO Profile**: A retrained Profile-Guided Optimization profile providing ~3% lower frame times.
- **AMD Vega / Polaris Compatibility (#305)**: Shaders reading GPU barycentrics now utilize AMD's VK_AMD_shader_explicit_vertex_parameter on older drivers.
- **Native Game Patches & Cheats**: Direct cheat file support via `_Patches/PPSA01341.json`.

## Preserved RTX 30 integrations

**1920×1080 DLSS → 2560×1440 output + 2× frame generation**, original **3840×2160 UI/HUD canvas (`InterfaceScope`, `InterfaceTransformFor`)**, shared SR/FG frame tokens (`g_upscale_token`), 64 MiB VMA heap sizing, and DXGI VRAM-pressure sampling/cache trimming are strictly retained.

**Automated validation:** Focused tests passed: `demons_souls_output_tests`, `demons_souls_output_integration_tests`, `page_manager_tests`, `save_data_backup_tests`, `vulkan_recording_tests`, and all 4 `LauncherCommandTests.ps1` permutations. The binary pair is hash-identical to the verified build output.

**Installation:** extract the portable ZIP and use `KytyLauncher.exe` or `launcher.cmd`. Standard app/VC runtime DLLs and guides are included. **NVIDIA-signed Streamline and OpenDLSSG Enabler are separate prerequisites**; keep `version.dll` beside the actual emulator executable. Enable Windows HAGS and restart. Game files, saves, and personal settings are excluded.

See the [RTX 30 setup guide](https://github.com/giangero1/KytyPS5-RTX30-FrameGen/blob/feature/rtx3070-dlssg/docs/RTX30_FRAME_GEN_GUIDE.md) and [safe future-update playbook with a reusable request](https://github.com/giangero1/KytyPS5-RTX30-FrameGen/blob/feature/rtx3070-dlssg/docs/UPSTREAM_UPDATE_PLAYBOOK.md).

---

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

Your precompiled shaders stay valid: the shader translation is the same as in v20261116.

- **Malwarebytes no longer flags the launcher** (#346, PR #348, credit @DevList1454): its machine-learning check
  reported every `KytyLauncher.exe` since v20261105 as "MachineLearning/Anomalous". It reacted to compressed image data
  inside the exe (the keyboard prompt textures), which a program unpacks and writes like a dropper would. They are
  stored uncompressed now; the launcher works the same, and the exe is 3.3 MB instead of 0.35 MB (the download barely
  changes).
- **Parallel translation (`KYTY_SPECULATE=4`, experimental)**, a round by @CarpetHead (PR #342):
  - crossing into a new area stutters much less: in a fixed 18 s run across an area boundary in 1-3 (6 runs each, RTX
    5090), the ten frames after the crossing spent 139 -> 77 ms above the calm frame time, and the crossing frame took
    66 -> 55 ms. The commit of a translation now finishes more of its own work instead of stopping, its checks are
    exact, and the game's memory calls at a crossing no longer wait on the GPU thread.
  - with "Up to 120 fps" on, 13–29% of the frames at 5-1 were held to two refreshes (a frame a worker translated was not
    marked as the 3D view); now none are.
  - about a quarter of the boots had a storm of write faults (one page faulting every frame, about 3 fps lost per 100
    faults a frame); it no longer happens in this mode.
  - the write guard's wait for a written label no longer takes its lock over and over, which could hold up the upload
    worker that releases the label (seen as frames of 40–280 ms waiting for uploads in some test runs; they did not
    come back in the later runs with or without the change, so this one is unmeasured).
  - Linux (PR #345, credit @Bios2000): after a load step the game could crawl at about 5 fps or freeze. A page the write
    guard had protected was left without access and without an owner; this was also the cause of #289.
  - the default mode runs as before: everything above is off there.
- **Linux: NVMe power saving** (#343, credit @Bipboop-beep): if the game stalls for up to seconds at a time while it
  loads textures, with the SSD busy but reading little, the drive's power saving (APST) may be waking it too slowly. The
  package README now names the kernel parameter that turns it off (`nvme_core.default_ps_max_latency_us=0`).
- **Performance profile retrained** on the new code, so the default mode keeps its speed (RTX 5090, 120 Hz cap): the
  1-1 start 68.0 fps against 66.1 before this round, the 1-1 arena 114.6 against 112.9 with the old profile, Latria the
  same.
- **Thanks** to @CarpetHead for the parallel translation round, @Bios2000 for the Linux runs that found the write guard
  bug, @DevList1454 for tracking down the Malwarebytes flag, @Bipboop-beep for the NVMe measurements, and @Osbrin for
  the report.

## New in v20261116

Your precompiled shaders stay valid: the shader translation is the same as in v20261114.

- **Long sessions: the 0.2–15 s freezes fixed**: about once every one to two and a half hours of play, the game could
  stop for up to 15 s while about a hundred pipelines compiled at once (the pass that draws the world's surfaces had
  lost one of its six targets). The cause was in Windows: when one of the game's memory copies took a fault, Windows
  sometimes resumed it with the upper halves of its AVX registers cleared, and the copy wrote 32 zero bytes into the
  game's render state. A copy that took a fault is now made again. Two hours of world changes after the fix: no freeze.
- **Fewer stutters after world changes, faster loads**: the game's threads hand their memory maps, unmaps and
  readbacks to the emulator's GPU thread and wait for it, and that thread did not take them while it waited for a
  frame to be shown (at 60 fps, every frame it finished early). Now it does. Same session, switched on and off at each
  world change (RTX 5090, 60 fps): the game's threads waited 16.2 -> 0.9 ms a frame in play, the 1% low went
  53.0 -> 55.4 fps (25.6 -> 37.2 fps in the seconds when the game frees a world's textures), frames of 45 ms or more
  14 -> 6, and the world appeared 6.2 -> 5.0 s after a load. `-Set KYTY_SERVE_IN_FLIP_WAITS=0` brings the old
  behaviour back.
- **No more whole-emulator pauses while a world loads** (Windows): in the seconds after a world change, every thread of
  the emulator stopped together for 3–44 ms, several times a load. The upload worker, kept on the performance cores at
  the game's priority, lost its CPU to the game's threads while it held a lock that every memory map of the game needs.
  It now runs just above them: 22 such pauses in 6 loads before, none after; standing in 1-1 the frame rate is the same
  and the 1% low 65.1 -> 65.9 fps. `-Set KYTY_UPLOAD_PRIORITY=0` brings the old behaviour back.
- **Precompile: says what it is doing** (#334, credit @shark20619): it printed nothing for minutes. It now prints each
  step (the shader list, the plans, the shards, the merge) with the time since it began, and its progress line reads
  "shards: 12 of 1096 done (1%), 8 running, 1076 waiting". Windows and Linux.
- **Parallel translation (`KYTY_SPECULATE=4`, experimental)**:
  - no more render-thread stalls of 1–6 s: a lock the render thread shares with the translation workers only spun, and
    a worker that lost its CPU while holding it kept the render thread spinning until Windows boosted the worker, about
    four seconds later (about 20 frames of 0.3–5.7 s in 43 minutes on the test PC). A thread waiting for it now sleeps
    after a short spin: none in 36 minutes. The default mode measured the same.
  - the scout stands down while it is on (PR #339, credit @Bios2000): the workers translate the draws it prepares.
    Linux, 1-1: +3.7% fps on 16 CPUs, +8.0% on 12.
  - Linux (PRs #336, #337, #338, credit @Bios2000): the workers read game memory directly instead of one system call a
    read; four bytes of the draw state left uninitialized made some runs refuse nearly every draw in this mode; threads
    started by the render thread now leave its CPUs; long thread names are set again (15 characters at most).
- **Thanks** to @Bios2000 for the pull requests and to @shark20619 for the report and the tests.

## 本版本的变化

已有的预编译着色器继续有效：着色器翻译结果和 v20261116 相同。

- **Malwarebytes 不再误报启动器**（#346，PR #348，感谢 @DevList1454）：从 v20261105 起，Malwarebytes 的机器学习检测
  把每个版本的 `KytyLauncher.exe` 都报为 "MachineLearning/Anomalous"。触发它的是 exe 里压缩存放的图片数据（键位提示
  贴图）：程序把一段压缩数据解开再写到磁盘，看起来像是释放器。现在这些贴图不再压缩存放；启动器功能不变，exe 从 0.35 MB
  变成 3.3 MB（下载包大小几乎不变）。
- **并行翻译（`KYTY_SPECULATE=4`，实验功能）**，@CarpetHead 的一轮改进（PR #342）：
  - 进入新区域时的卡顿明显减少：在 1-3 跨越区域边界的固定 18 秒路线上（每边 6 次，RTX 5090），跨越后 10 帧比平稳帧多出的
    总时间 139 -> 77 ms，跨越那一帧 66 -> 55 ms。翻译的提交现在会把更多工作自己做完而不是中途停下，检查改为精确判断，
    跨越时游戏的内存调用也不再等待 GPU 线程。
  - 开启"最高 120 帧"时，5-1 有 13–29% 的帧被压到两个刷新周期（工作线程翻译的那一帧没有被标记为 3D 画面）；现在没有了。
  - 大约四分之一的启动会出现写入异常风暴（同一页每帧都触发异常，每帧每 100 次异常约损失 3 fps）；这个模式下不再出现。
  - 写保护在等待已写入的标签时不再反复加锁，以前这可能拖住负责解除标签保护的上传线程（部分测试运行中出现过 40–280 ms
    等待上传的帧；之后的运行中无论有没有这项改动都没有再出现，所以这一项没有测量数字）。
  - Linux（PR #345，感谢 @Bios2000）：读图的某一步之后，游戏可能只有约 5 fps 或直接卡住：写保护保护过的某一页被留成了
    不可访问且没有任何组件负责恢复；这也是 #289 的原因。
  - 默认模式不受影响：以上改动在默认模式下都不生效。
- **Linux：NVMe 节能**（#343，感谢 @Bipboop-beep）：如果读贴图时游戏一次卡顿最长达数秒，而硬盘很忙却几乎没读多少数据，
  可能是硬盘的节能功能（APST）唤醒太慢。Linux 包的 README 里现在写了关闭它的内核参数
  （`nvme_core.default_ps_max_latency_us=0`）。
- **重新训练了性能配置文件（PGO）**，适配新代码，默认模式保持原来的速度（RTX 5090，120 Hz 上限）：1-1 起点 68.0 fps，
  这一轮改动之前是 66.1；1-1 竞技场 114.6，旧配置文件下是 112.9；Latria 不变。
- **感谢** @CarpetHead 的并行翻译改进，@Bios2000 在 Linux 上的测试找出了写保护的问题，@DevList1454 查明了 Malwarebytes
  误报的原因，@Bipboop-beep 提供了 NVMe 的测量数据，以及 @Osbrin 的报告。

## v20261116 的变化

已有的预编译着色器继续有效：着色器翻译结果和 v20261114 相同。

- **长时间游玩：0.2–15 秒的卡死修复了**：大约每玩 1 到 2.5 小时，游戏可能会停住最多 15 秒，同时一次编译约一百个管线
  （绘制场景表面的那一遍渲染丢了 6 个渲染目标中的一个）。原因在 Windows：游戏的某次内存复制遇到缺页等异常时，Windows
  有时恢复执行后会把 AVX 寄存器的高半部分清零，于是这次复制往游戏的渲染状态里写进了 32 个字节的 0。现在遇到异常的复制
  会重新做一遍。修复后连续两小时切换世界：没有再卡死。
- **切换世界后的卡顿更少、读图更快**：游戏线程把内存映射/取消映射、回读交给模拟器的 GPU 线程处理并等待其完成，而 GPU
  线程在等待画面显示时（60 fps 下每一帧提前做完时都会这样）不处理这些请求；现在会处理。同一次运行中每次切换世界时交替
  开关对比（RTX 5090，60 fps）：游玩时游戏线程每帧等待 16.2 -> 0.9 ms，1% low 53.0 -> 55.4 fps（游戏释放一整个世界的
  纹理那几秒 25.6 -> 37.2 fps），45 ms 以上的帧 14 -> 6，读图后出现世界画面的时间 6.2 -> 5.0 秒。
  `-Set KYTY_SERVE_IN_FLIP_WAITS=0` 恢复旧行为。
- **读图时不再整个模拟器一起卡住**（Windows）：切换世界后的几秒里，模拟器的所有线程会一起停 3–44 ms，每次读图好几次。
  原因是上传线程固定在性能核上、优先级和游戏线程一样，拿着一把游戏每次映射内存都要用的锁时被游戏线程挤下 CPU。现在它的
  优先级略高于游戏线程：以前 6 次读图有 22 次这样的停顿，现在没有；1-1 站立时帧率不变，1% low 65.1 -> 65.9 fps。
  `-Set KYTY_UPLOAD_PRIORITY=0` 恢复旧行为。
- **预编译：会显示正在做什么**（#334，感谢 @shark20619）：以前好几分钟没有任何输出。现在每一步（列出着色器、规划、
  分片编译、合并）都会打印出来并带上已用时间，进度行改为 "shards: 12 of 1096 done (1%), 8 running, 1076 waiting"。
  Windows 和 Linux 都有。
- **并行翻译（`KYTY_SPECULATE=4`，实验功能）**：
  - 渲染线程不再卡 1–6 秒：渲染线程和翻译工作线程共用的一把锁以前只会自旋，持锁的工作线程被挤下 CPU 时，渲染线程会
    一直自旋，直到约 4 秒后 Windows 提升那个工作线程的优先级（测试电脑上 43 分钟内约 20 帧 0.3–5.7 秒）。现在等锁的线程
    短暂自旋后就睡眠：36 分钟内一次也没有。默认模式测下来不变。
  - 开启时 scout 线程会停下（PR #339，感谢 @Bios2000）：它准备的绘制本来就由工作线程翻译。Linux 1-1：16 个 CPU +3.7%，
    12 个 CPU +8.0%。
  - Linux（PR #336、#337、#338，感谢 @Bios2000）：工作线程直接读取游戏内存，不再每次读取都走一次系统调用；绘制状态里有
    4 个字节未初始化，导致某些运行中这个模式几乎拒绝所有绘制；渲染线程启动的线程现在会离开渲染线程的 CPU；较长的线程名
    也能设置了（最多 15 个字符）。
- **感谢** @Bios2000 的 PR，以及 @shark20619 的报告和测试。
