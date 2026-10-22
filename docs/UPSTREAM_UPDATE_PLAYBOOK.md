# Safely update the RTX 3070 fork from upstream

This is the maintenance guide for [giangero1/KytyPS5-RTX30-FrameGen](https://github.com/giangero1/KytyPS5-RTX30-FrameGen).
Use a candidate branch and candidate binaries first. A successful merge/build alone does not establish working DLSS or frame generation.

## Known-good baseline: 22 October 2026

- Upstream: [chenxiao07/KytyPS5 v20261022](https://github.com/chenxiao07/KytyPS5/releases/tag/v20261022), commit `bfa6558f41d3f5d706f59e86bf04dcc74ace9d02`.
- Verified binary source: `f075b460`; upstream merge: `598219f9`; retained test branch: `test/upstream-v20261022-merge`.
- Promoted release: `v2026.10.22-upstream-merge`. Resolve this immutable annotated tag for the final documentation/packaging commit. `BUILD-INFO.txt` records both commits.
- Emulator SHA256: `af817c4325dd90fc11c98d1d4e2651d05e3927b81b7d9f8ff68c37dea5085842` (23,539,712 bytes).
- Precompiler SHA256: `61d434f42c69ca5a5f022c448c1de473210ffb6971f82469f406663edffad720` (12,115,968 bytes).
- Launcher: `KytyLauncher.exe` built with MSBuild .NET 4.7.2 (`_Build/launcher/KytyLauncher.exe`, 146,944 bytes).
- Windows x64, Clang-CL 19, ThinLTO, `KYTY_MARCH=x86-64-v3`, NVIDIA Streamline headers enabled.
- User validated gameplay, save backups, DLSS and OpenDLSSG 2x FG on RTX 3070 with driver 617.42 shader precompilation. Boletaria 85–95+ displayed FPS is a user observation, not a controlled benchmark or a guarantee. Existing screenshots show 43 base / 85 displayed FPS.

Local installation: `D:\Games\demon souls port pc`; repository: its `KytyPS5_Source` subfolder.
`origin` is **chenxiao07 upstream**, not your fork. `myfork` is **giangero1/KytyPS5-RTX30-FrameGen**.
The fork's default branch is `feature/rtx3070-dlssg`; development branch is `fix/dlss-output-ui-canvas`.

## Copy and paste this request for the next update

```text
Read docs/UPSTREAM_UPDATE_PLAYBOOK.md in KytyPS5_Source before making changes.
Check chenxiao07/KytyPS5 for its latest stable release and compare it against our
last verified upstream baseline. Safely merge that release into a new test branch
starting from our current giangero1/KytyPS5-RTX30-FrameGen branch; do not reset our
fork to upstream or overwrite the installed working build.

Preserve RTX 3070 OpenDLSSG support, 1920x1080 DLSS -> 2560x1440 output, the original
3840x2160 UI/HUD canvas, shared SR/FG frame tokens and DXGI VRAM-pressure handling.
Back up Git refs, binaries, launcher scripts/settings and saves before testing.
Build with Clang-CL 19, ThinLTO, x86-64-v3 and Streamline headers, and put uniquely
named candidate EXEs in the game root beside version.dll. Never replace the proxy
or Streamline runtime just to make an upstream merge pass.

Run the seven core tests and native/DLSS x FG-off/2x launcher checks. Report conflicts,
source changes and remaining limitations. Use the candidate launcher for gameplay
verification: successful SR/FG logs, menus/inventory, save backups and memory behavior.
Keep default binaries, branches and releases unchanged until candidate validation
is complete. Then promote only the verified binaries with rollback backups, update
both myfork branches, package checksummed assets, credit upstream and publish a new
*-upstream-merge release. Update this playbook and its installation-folder copy.
```

## Update workflow and protected integrations

1. Inspect `git status`, remotes, branch tips, upstream release notes and runtime versions/hashes. Preserve local backup refs and copy binaries/scripts/settings. With the game closed, copy `_SaveData` to a separate timestamped local backup; never publish it.
2. Fetch upstream without replacing existing tags. Confirm the intended release tag's commit against upstream. Create `test/upstream-<tag>-merge` from the current fork, then merge upstream while preserving history. Avoid force pushes and unrelated cherry-picks.
3. Review conflicts by behavior, not by blindly choosing ours/theirs. Pay particular attention to `demonsSoulsRenderSize`, `hud-anchor`, presentation/swapchain code, `frame-gen`, launcher routing and texture-cache/memory policy.
4. Preserve **1920x1080 scene inputs -> 2560x1440 SR output**, original **3840x2160 UI layout/composition**, render-resolution depth/motion, correct FG color dimensions, shared SR/FG frame token with distinct viewport IDs, and adapter-matched DXGI pressure handling. Pressure starts below 512 MiB headroom, escalates after two seconds and clears above 768 MiB for two seconds; retain GPU-completion/data-preservation checks.
5. Build both emulator and precompiler using `build-windows.cmd <target>` with `$env:KYTY_CMAKE_ARGS='-DKYTY_MARCH=x86-64-v3'`. Confirm Streamline headers are found and FG is actually compiled in. Keep compiler/dependency choices stable during the merge.
6. Copy the pair to uniquely named candidate EXEs in the installation root. **Keep the emulator beside the installed `version.dll`**; changing only the working directory is insufficient. Select it with `launcher.ps1 -EmulatorExe '<root>\candidate.exe'`; use `-DryRun` first. Native and DLSS must resolve the same candidate. If testing precompile, explicitly select the matching candidate precompiler through `precompile-windows.ps1 -Exe`; the launcher otherwise selects the normal precompiler.
7. With the game closed, run the tests below. Then validate gameplay using the candidate. Preserve existing settings; record any temporary profile changes and restore them. Do not publish until validation is complete.
8. Close all Kyty processes. Back up and replace the normal binary pair, hash-check both, and install the unified source launcher if an older installed launcher still reroutes DLSS to an old candidate. Preserve the original settings file byte-for-byte.
9. Fast-forward both fork branches to the verified release commit; reject divergent remote updates for review. Push explicitly to `myfork`, preferably atomically. Use a fresh annotated `v...-upstream-merge` tag; never move published tags.

## Validation gates

Run from the source directory with the emulator closed (`ctest` may require the Visual Studio CMake bin directory on PATH):

```powershell
ctest --test-dir _Build/windows --output-on-failure -j 1 -R '^(demons_souls_output|launcher_commands|save_data_backup|page_manager|command_scheduler_timeline|host_image_allocation|texture_cache_depth_readback)$'
powershell -NoProfile -ExecutionPolicy Bypass -File tests/LauncherCommandTests.ps1 -EmulatorExe 'D:\Games\demon souls port pc\candidate.exe'
```

- Check native1440+FG, DLSS1080 without FG, and DLSS1080+FG with the same executable, warmed scene and camera. Confirm `1920x1080 -> 2560x1440`, increasing successful SR evaluations, no recurring SR fallback, and FG `presented=2` with base/displayed FPS distinguished.
- Check menus, inventory/equipment, quantities/highlights, loading/fades, HUD and lock-on. Repeat inventory transitions; watch VRAM usage/budget and memory recovery. Check save backup logs/directories and F9 pause/resume. Never damage real saves to test recovery; use isolated fixtures.
- Preserve the documented pre-existing BC3 atlas overlap failure and untested configurations in the validation record. Seven passing focused tests do not mean the complete suite passes. Test 1080p/4K/ultrawide separately before making claims about them.
- If any candidate check fails, retain the working installation and report the failure. Do not treat launch success or feature availability alone as proof of working FG.

## Package and publish the exact verified build

`tools/package-windows-release.ps1` requires an unused tag, binary source commit and both independently recorded hashes. Commit docs/packaging first; it rejects compiled-source changes since the binary commit and dirty source trees.

```powershell
.\tools\package-windows-release.ps1 -Tag 'v2026.10.20b-upstream-merge' -BinarySourceCommit c01f9217 -EmulatorSha256 2d00df6c7430241ef68153e4966627de7cc574cc0a48d75b8f119656feae0203 -PrecompileSha256 ac157925d75c2ea1b542a3d47303a138ac128e89352879469de2b2f89ea20ea3
```

For a future release, replace the tag, source commit and hashes with the newly verified values. Output is `_Build/package/<tag>/`; it refuses to overwrite that directory. Extract into a fresh folder and verify every manifest entry, ZIP checksum, screenshot/guide links, normal runtime DLLs and launcher commands before uploading.

The ZIP includes standard app/VC runtimes and six reference screenshots, but excludes Streamline/OpenDLSSG, game files, saves, caches and personal settings. FG users must separately install their compatible NVIDIA-signed Streamline runtime and OpenDLSSG proxy beside the extracted emulator; HAGS must be enabled and Windows restarted.

`v*-upstream-merge` tags are excluded from automatic CI publishing so locally verified binaries are not replaced or supplemented by a different build. Ordinary tags/manual workflows retain their existing behavior. Publish explicitly with `gh --repo giangero1/KytyPS5-RTX30-FrameGen`: prepare notes as a file, create a draft with `--verify-tag`, attach ZIP and `SHA256SUMS.txt`, verify assets, then publish with `--draft=false --latest`. Credit/link the exact upstream release and distinguish upstream work from fork integrations. Verify both remote branch heads and the peeled tag commit afterwards.

## Rollback

Close the emulator/launcher. Restore **both** executables from the same `_Backup_UpstreamPromotion_<timestamp>` folder, plus `launcher.ps1` if launcher routing changed. The folder contains `ROLLBACK.md`, original scripts/settings and a hash inventory. Do not delete/restore saves or caches merely to roll back binaries, and do not overwrite a published release/tag.

The initial promotion also keeps `kyty_emulator_v20261018_backup.exe` and local Git refs `backup/pre-v20261019-promotion-fix` and `backup/pre-v20261019-promotion-default`. To undo a published integration, prepare reviewed revert commits on a new branch rather than resetting shared branches. Retain the old release for recovery.
