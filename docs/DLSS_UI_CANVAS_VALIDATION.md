# DLSS output and original UI canvas: validation

## Implementation

The v1.07 settings record separates processing size (+0x0c) from display/interface size (+0x14). Change the SR context and processing dimensions, retain the original 3840×2160 display pair, output tables and shared float layout constants. The game draws its interface at 4K and samples that surface during final composition with the reduced scene. Do not apply a second 16:9 UI viewport/scissor/mask scale. Existing ultrawide anchoring remains separate.

All output patch sites are discovered before guest writes; missing signatures reject the modified Pro path and retain TAA. Output is selected at startup from configured screen dimensions and aspect, aligned to existing requirements, capped at 2160 lines; the Pro path requires at least 1080. Render height is clamped to processing height even when unchanged or omitted. Cinematic layout constants remain intact.

SR and FG share a frame token reserved before queued recording, with separate feature viewport IDs. Depth/motion remain at render resolution and FG color at backbuffer resolution. Runtime evaluation counters distinguish success from stretch fallback. Streamline's post-evaluation out-of-budget warning retains the completed SR image while remaining visible diagnostically.

The separate memory snapshot API records validity/source/process usage/budget/headroom. DXGI is adapter-matched; Vulkan budget reporting is the fallback. Sampling, hysteresis, completed-pool trimming, normal-age then 120-frame LRU escalation and bounded preservation downloads retain existing lifetime checks. Logical retirement and actual allocation frees are logged separately. The active working set can still exceed budget.

## Local checks (2026-10-19)

| Check | Result |
| --- | --- |
| Release emulator with existing Windows clang-cl/Ninja toolchain | Passed |
| Output selection, interface transforms, atomic patch batches, warning classification, memory hysteresis/unavailable queries/readback limits | Passed (`demons_souls_output`) |
| Loader integration using local v1.07 eboot, no distributed game data | All 11 scenarios passed |
| Scheduler timeline / real GPU deferred image release / depth readback | Passed |
| Launcher command generation | Four combinations (native/DLSS × FG off/2×) pass; every command uses the same explicit executable |
| Captured golden startup, candidate startup/gameplay/latest gameplay | Original 3840×2160 UI attachment/viewport and final composite consuming that attachment with a 2560×1440 scene passed |
| Broader `texture_cache_image_overlap` | Fails at `RenderExecutorStencilBindingDiscovery`: compressed BC3 atlas storage reuse |

The BC3 failure was reproduced with the existing branch's HEAD texture-cache implementation substituted, with the game closed. That narrows it beyond the new texture-cache changes, but is not a clean full-tree baseline certification; it remains an outstanding regression-suite failure. The source was restored and the candidate rebuilt afterwards. GPU test builds retain existing compiler warnings about missing return/noreturn annotations in unrelated headers.

A fresh same-executable native launcher startup was attempted; it reached startup rendering and ended before the timed smoke check completed. This is not a completed same-build two-configuration gameplay acceptance test. Automated launcher command checks passed for both modes. Earlier successful gameplay and the supplied screenshot pair used local candidate/normal executable routing, so their FPS difference cannot be attributed solely to render resolution.

## Runtime evidence and practical limits

Earlier candidate logs recorded 1920×1080 → 2560×1440, at least 12,600 successful SR evaluations, zero failures/stretch fallbacks and gameplay FG `presented=2`. A 15-second guest-frame sample measured 41.80 base FPS, 34.70 FPS 1% low, 28.0 ms p99, 30.6 ms longest frame and no frames over 50 ms. Approximately 83.6 displayed FPS follows from two presented frames; it was not independently measured by that sampling command. This is one short sample, not a controlled comparison or minimum guarantee.

Usage still reached 7212 MiB against a 7074 MiB Windows budget. A 63.75 MiB interface attachment does not represent total pipeline overhead. Upload counters are emulator uploads, not measured driver paging. The calculated driver-memory remainder is not fixed DLSS overhead. No texture-quality settings or runtime DLLs were changed for these tests.

The user reports correct menus/inventory/gameplay. Captures disable some accelerated drawing, so capture checks alone are insufficient. Still pending: ten identical inventory/equipment cycles with measurements after cycles 5 and 10; matched-camera, same-build native1440+FG / DLSS1080-no-FG / DLSS1080+FG averages; full visual checks of masks, quantities, highlights, lock-on, fades and loading; runtime 1080p/4K/ultrawide acceptance. Release as experimental with these limits visible.

## Reproduction

Build `kyty_emulator`, `demons_souls_output_tests`, `demons_souls_output_integration_tests` and `shader_recompiler_compute_tests` with `build-windows.cmd <target>`. Run:

```powershell
ctest --test-dir _Build/windows --output-on-failure -j 1 -R '^(demons_souls_output|launcher_commands|command_scheduler_timeline|host_image_allocation|texture_cache_depth_readback)$'
.\_Build\windows\demons_souls_output_integration_tests.exe 'C:\your-local-dump\eboot.bin'
python tests/VerifyUiCanvasCapture.py 'C:\your-local-capture\frame-0.jsonl'
```

Run GPU tests with the emulator closed to avoid reserving another large guest-memory region. The integration executable uses a private in-memory copy and never changes the game file. Capture/game data and developer runtime files are not included in the source changes or release. For rollback, retain your previous package in a separate folder; do not overwrite saves or shader caches to roll back an executable.
