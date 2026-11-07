# KytyPS5 portable test build (Windows)

A Windows test build of the Kyty PS5 emulator. So far it has been tuned only for
**Demon's Souls (the PS5 remake)**. This `v2026.11.06-upstream-merge` package contains the exact locally built and verified binaries from `06cf66ce` (Clang-CL 19, ThinLTO, AVX2, Streamline headers), from
https://github.com/giangero1/KytyPS5-RTX30-FrameGen. `BUILD-INFO.txt` identifies both the binary source and final release commit; verify the external ZIP checksum and internal `SHA256-MANIFEST.txt`.

Thanks to [chenxiao07/KytyPS5 v20261106](https://github.com/chenxiao07/KytyPS5/releases/tag/v20261106) and its contributors for native stereo sound mixing, the Scout thread (+5-11% FPS), NVIDIA empty fragment shader stencil optimization, dynamic 8 GB VRAM budget handling, and F11 swapchain fixes. Backups are under `_SaveData/.kyty-backup/PPSA01341/<slot>/`; they complement independent save backups.

The RTX 3070 DLSS/output/UI, shared SR/FG token and VRAM-pressure integrations are preserved. See [release notes](docs/RELEASE-NOTES.md) and the [safe future-update playbook](docs/UPSTREAM_UPDATE_PLAYBOOK.md).

## RTX 3070: 1080p DLSS to 1440p with 2× frame generation

This experimental build supports **1920×1080 rendering → 2560×1440 DLSS → 2× FG**
while retaining the original 4K menu/HUD canvas. Native 1440p + 2× FG is also available.
Use the same packaged `launcher.cmd` and emulator for either configuration.

**Enable hardware-accelerated GPU scheduling (HAGS) in Windows Settings → System →
Display → Graphics → Change default graphics settings, then restart Windows.**
RTX 30 FG requires the separate OpenDLSSG compatibility setup and NVIDIA-signed Streamline runtime;
these runtime/proxy DLLs are not bundled. The feature is not ready to use until they are installed.
Follow [the setup guide and screenshot comparison](docs/RTX30_FRAME_GEN_GUIDE.md).

For DLSS select Resolution **2560×1440**, Render height **1080**, Upscaler **DLSS (PS5 Pro path)**,
Frame generation **2×**. For native select Render height **1440 (PS5)** and Upscaler **Game (PS5)**.
Each upscaler remembers its own render height. DLSS signature validation was tested with v1.07;
unsupported layouts fall back to TAA with a diagnostic.

User bridge snapshots show **43 real / 85 displayed FPS** for DLSS and **30 / 60** for native.
These are observations, not controlled averages or guaranteed frame rates. FG uses GPU time and
memory and can lower base FPS; frame times do not measure end-to-end input latency.
Eight-gigabyte cards can still exceed the Windows video-memory budget, and long-session memory
stability is not certified. See [validation limits](docs/DLSS_UI_CANVAS_VALIDATION.md).

## Supported game versions

| Item | Supported |
| --- | --- |
| Game | Demon's Souls |
| Title ID | PPSA01341 |
| Content ID | EP9000-PPSA01341_00-DEMONSSOULS00000 |
| Versions | 01.007.000 (1.07) and 01.005.000 (1.05) |

Both versions get the same performance patches. Each version has shader caches of its own (versions do
not share shaders), so the shader precompile is done once per version. Other versions, other regions
and other games have not been tested and may not run or may fail. At start-up the launcher reads the
game's `sce_sys\param.json` and warns when the version is not one of these.

This dump is the European release, whose language list has no Chinese, Japanese or Korean; the game files
hold every language, and one value turns the others on: see [Game language](#game-language-chinese-japanese-korean-and-others).
Boletaria's start on an RTX 5090, 69.8 fps at 4K, 78.3 at 1080p. It suits 1080p screens; on larger ones the
picture is softer than the 4K output's. With "DLSS (PS5 Pro path)" too, the game's PS5 Pro mode makes a
1920x1080 output instead of its 4K one: DLSS turns the 3D at the render height (360 = Ultra Performance, 540 = Performance, 630 = Balanced, 720 and 900 = Quality, 1080 = DLAA)
into the 1080p picture, and the game finishes it at 1080p, HUD included. For 1080p screens and graphics cards
that find DLSS's 4K output too heavy (RTX 20/30): the game's own 1080p mode has no upscaler (its 3D, anti-aliasing
and effects at the render height, stretched at the end). On an RTX 5090 at Boletaria's start (its processor the
limit there): DLSS from 720 to 1080p 75.2 fps, DLSS from 1080 to 4K 68.8, the game's TAA at 1080p 76.8.
"1440p" (with DLSS or FSR 3 only; -Upscaler dlss -OutputHeight 1440 on a 
un.cmd line, contributed by Tutuzaooo)
makes that output 2560x1440, for 1440p screens: the upscaler's own picture at the screen's size, neither the 4K one
scaled down nor the 1080p one scaled up, the HUD and menus in the game's 4K layout drawn at 1440 (as the 4K output's,
scaled down; `-Set KYTY_OUTPUT_LAYOUT=hd`: the game's HD layout scaled up, as before). Its render heights:
1440 = DLAA, 960 = Quality, 840 = Balanced, 720 = Performance, 480 = Ultra Performance. With the game's TAA the
output stays 4K.

"Frame generation" (DLSS, 2x to 6x; official native support on NVIDIA RTX 40 or newer, 3x and up RTX 50; this
fork's RTX 30 compatibility setup with OpenDLSSG is validated at 2x) uses NVIDIA's Streamline
runtime, which this package does not include: download the Streamline SDK (a release zip from
https://github.com/NVIDIA-RTX/Streamline/releases), unpack it and choose its folder with the launcher's
"Streamline..." button (only files signed by NVIDIA are loaded). The frames shown are the game's times the
multiplier; with VSync they are capped at the display's refresh rate (a 60 Hz display at 2x runs the game at 30 fps).
"FSR 3.1 frame generation" (2x, any graphics card, AMD included; `run.cmd -Fsr`) uses AMD's FidelityFX runtime,
which this package does not include either: take `amd_fidelityfx_vk.dll` from the FidelityFX SDK 1.1.4
(`PrebuiltSignedDLL` in https://github.com/GPUOpen-LibrariesAndSDKs/FidelityFX-SDK/tree/v1.1.4) and put it next to
`kyty_emulator.exe` (only a DLL signed by AMD is loaded); the launcher's FSR checkbox is enabled then. It takes the place of
DLSS frame generation.
"Overlay" (Off by default) shows the emulator's own panel at the top right of the game image: "Frame rate"
the frames per second and a graph of the last frames' times (60 and 30 fps lines, hitches in red, the 1% low
and the longest frame of the last 10 seconds); "Detailed" adds the render thread's time in each frame (orange),
the graphics card (load, clock, temperature, power and video memory on NVIDIA cards), the processor's load
with a bar for each core, and the memory in use. (The overlays seen in videos with temperatures and voltages
of every part come from separate tools such as MSI Afterburner's RivaTuner Statistics Server.) In the
game, F8 switches the panel between Off, "Frame rate" and "Detailed", whatever the launcher started it with
(also when started with `run.cmd`).
"Safe GPU mode" (off by default) prevents the crashes and blue screens that come from the graphics card
hanging ("device lost", the screen going black while the graphics driver restarts, a VIDEO_TDR_FAILURE blue
screen), but **the frame rate is about 37% lower**. The emulator normally lets the graphics card overlap
Demon's Souls' compute work; in this mode it runs the steps one after another. On a laptop RTX 3080 (driver
566.07) that had these hangs, 3 of 4 runs through Boletaria hung without it and none of 3 with it, at 17 fps
instead of 27. It does not help the MEMORY_MANAGEMENT blue screens of a full RAM (see RAM under System
requirements). Leave it off when the game runs without such crashes; a current graphics driver can help as well.
(With `run.cmd` or `run-windows.ps1`: `-Set KYTY_COMPUTE_CHAIN=0`.)
Its settings and language list follow the KytyPS5 launcher (`src/launcher` in the source, by the
KytyPS5 developers).

Or **double-click `run.cmd`** to start with the default settings (do not double-click
`kyty_emulator.exe` directly: it would lack its launch options).

The first launch goes like this:

1. Choose the game folder (see above).
2. The console lists the game's shaders from its files (once; nothing to install for it).
3. A dialog shows whether the game version is a supported one and offers to precompile the shaders
   first (see the next section).
4. The console window makes the input files for the background shader preparation (once per
   graphics card and driver version): about 50 seconds on the test PC's 22 threads, a few minutes on
   PCs with fewer threads.
5. The game window appears. The start-up screen shows the loading progress; in the game, the top
   right corner shows "Preparing shaders xx%".

Later launches go straight into the game. The console window stays open and shows the game's log as
it runs; when the game crashes it shows the exit code and the end of the error log and waits for a key
(the log files are in the `logs` folder). To quit, close the game window.

Common options (add them after `run.cmd` on a command line, or put them into the "Target" of a
shortcut):

| Option | Effect |
| --- | --- |
| `-Fullscreen` | Full screen (F11 or Alt+Enter also toggle it in the game) |
| `-Fullscreen -AspectFit` | Full screen keeping 16:9, with black bars instead of stretching |
| `-Width 1920 -Height 1080` | The window size (default 2560×1440, shrunk when the screen is smaller) |
| `-Game <folder>` | The game folder (or a `.zar` archive of it) |
| `-Language 11` | The console language, as the PS5 numbers them (0 Japanese, 1 English (US), 11 Chinese (Simplified), ...; the launcher lists them) |
| `-Set KYTY_ALL_LANGUAGES=0` | Demon's Souls' language settings offer every language the package has, in every region: the text of 23 (a US game Russian, Japanese or Korean too) and the voices of 9 (English, French, German, Italian, Spanish, Japanese, Brazilian Portuguese, Latin American Spanish; British English speaks English); this keeps only the console region's |
| `-RenderHeight 1080` | The height the game renders its 3D at (360–2160; below 720 is meant for the 1080p output, `-HdOutput`, e.g. 540 = DLSS Performance there): lower is faster on a slow graphics card and less sharp. Without it the height follows the window, at most 1440 as on the PS5 (1080 on a 1080p screen) |
| `-HdOutput` | The game's own 1080p output (the launcher's "Game output: 1080p"): post-processing and HUD at 1920×1080, the 3D at most 1080 high; with `-Upscaler dlss`, DLSS makes the 1080p picture from it. Without it (and without `-RenderHeight`, `-Upscaler` or `-Aspect`) the game takes it by itself for a window or screen at most 1080 lines high; `-Set KYTY_HD_OUTPUT=0` keeps the 4K output |
| `-OutputHeight 1440` | With `-Upscaler dlss` or `-Upscaler fsr`: the PS5 Pro path's output at that height (1080–2159, 16:9; 2560×1440 for 1440, the launcher's "Game output: 1440p"), the HUD in the game's HD layout scaled to it; `-RenderHeight` picks the mode (1440 DLAA, 960 Quality, 840 Balanced, 720 Performance, 480 Ultra Performance) |
| `-Aspect 21:9` | The 3D rendered wider (`21:9` or `32:9`) for ultrawide displays; the HUD keeps its shape at the sides; in-game menus, movies and loading screens are stretched to the width (with `-AspectFit` too: movies, the title and loading screens keep their 16:9, with black bars at the sides) |
| `-Quality low` | How much the game draws: `low` or `lowest` (shorter shadows, simpler distant detail), faster where the processor limits the frame rate |
| `-TextureReduce 2` | Large textures without their largest levels: `1` half the size per side (the default with less than 12 GB of video memory), `2` a quarter, `0` whole |
| `-Fps120` | The game's frame rate up to 120 fps instead of 60 (the launcher's "Up to 120 fps") |
| `-Set KYTY_FLIP_RATE=1` | The game locked to a steady 30 fps, at the same game speed: half the work, even frame times; movies and menus stay at 60 (the launcher's Experimental "Lock to 30 fps") |
| `-PresentMode Fifo` | VSync on (the launcher's "VSync") |
| `-LowLatency` | Less input lag: one frame waiting for the screen instead of about two (the launcher's "Low latency") |
| `-Hdr` | HDR10 output when Windows' "Use HDR" is on for the display |
| `-Upscaler dlss` | DLSS super resolution through the game's PS5 Pro path (NVIDIA RTX, needs Streamline: see "Upscaler" above) |
| `-Upscaler fsr` | The same with AMD FSR 3, built in: any graphics card |
| `-FrameGen 1` | DLSS frame generation, the number of generated frames per rendered one (`1` = 2x, up to `5`; RTX 40 or newer, needs Streamline) |
| `-Gpu 1` | The graphics card on a PC with more than one: its number in the log's `Vulkan GPU <n>:` lines, or a part of its name (`-Gpu 4090`); give `precompile.cmd` the same `-Gpu`, so the shaders are compiled for that card. Without it the first discrete GPU, in the order Windows' graphics settings give |
| `-Set KEY=VALUE` | An emulator switch, e.g. `-Set KYTY_SHADER_PREFETCH=0` (several: `-Set A=1,B=2`); the issues name the ones worth trying |

The launcher has a box for these too (Advanced, "Command-line options"), written as after `run.cmd`: its `-Set`
pairs join the launcher's own (one of the same name replaces the launcher's), and another option replaces the
launcher's of the same name, e.g. `-RenderHeight 900` or `-Set KYTY_DLSS_PRESET=E`. If the game no longer starts,
clear the box.

## Game language (Chinese, Japanese, Korean and others)

The emulator makes the game's language settings offer every language in every release: the text of 23
(Chinese, Japanese and Korean too) and the voices of 9 (Settings > Language in the game; the voice
language from the title screen's settings). `-Set KYTY_ALL_LANGUAGES=0` turns that off. The edit below
is only needed with it off, or for a game version where the emulator does not find the language sets
(the log says so: `Demon's Souls languages: ...`).

The game files hold every language of the game, but the game offers only those of its release. It
picks the release from `userDefinedParam1` in the game folder's `sce_sys\param.json`, an index into a
table inside the game. This dump (the European release) has `2`, whose list has no Chinese, Japanese
or Korean. The emulator stores saves by the title ID in `param.json`, which does not change, so the
saves stay where they are.

| `userDefinedParam1` | Release | The game's language list |
| --- | --- | --- |
| `0` | Global | Every language: Arabic, Chinese (Traditional and Simplified), Danish, Dutch, English, English (UK), Finnish, French, German, Italian, Japanese, Korean, Norwegian, Polish, Portuguese, Portuguese (Brazil), Russian, Spanish, Spanish (Latin America), Swedish, Turkish, Thai |
| `1` | America | English, French, Portuguese (Brazil), Spanish (Latin America) |
| `2` | Europe (as dumped) | Arabic, Danish, Dutch, English (UK), Finnish, French, German, Italian, Norwegian, Polish, Portuguese, Russian, Spanish, Swedish, Turkish |
| `3` | Japan | English (UK), Japanese |
| `4` | Asia | Chinese (Traditional and Simplified), English (UK), Korean, Thai |

To switch:

1. Close the game. Copy `sce_sys\param.json` somewhere as a backup, open it with Notepad, change
   `"userDefinedParam1": 2,` to `0` (every language) or to `4` (the Asian release), and save it.
2. In the launcher, set "Console language" to the language you want (`run.cmd -Language 11` for
   Chinese (Simplified), `10` Traditional, `0` Japanese, `9` Korean): when the release has it, the game's
   list starts on it.
3. The game asks for its language only while there is no options save:
   - No saves yet (a new installation): it asks at the first start.
   - With saves: back up the `_SaveData` folder, then move the folder
     `_SaveData\PPSA01341\SAVEDATA0OptionsProfile0` out of `_SaveData` (to the desktop, say). The characters
     are in the `SAVEDATA0PlayerProfile...` folders and stay as they are; only the game's options
     (brightness, controls, camera, subtitles) go back to their defaults.
4. Start the game: "LANGUAGE SELECT" lists the languages of that release. Choose one and confirm with ×
   (J on the keyboard); the brightness and the speech and subtitle settings follow in that language.

中文说明：游戏文件里其实包含全部语言，只是游戏按 `sce_sys\param.json` 里的 `userDefinedParam1`（发行区域）决定
语言选择列表。本 dump 是欧版（值为 `2`，列表里没有中文、日文、韩文）。用记事本把它改成 `0`（全部语言，含简繁中文和日文）
或 `4`（亚洲版：简繁中文、英文、韩文、泰文）并保存（先备份原文件）；启动器的 "Console language" 选中文（简体）。
没有存档时首次启动就会出现语言选择；已有存档时，先备份 `_SaveData`，再把 `_SaveData\PPSA01341\SAVEDATA0OptionsProfile0`
这个文件夹移出 `_SaveData`（角色存档在 `SAVEDATA0PlayerProfile...` 里，不受影响，只有亮度、按键等游戏设置会恢复默认），
下次启动时在 "LANGUAGE SELECT" 里选择简体中文即可。存档按 `param.json` 里的 Title ID 保存，改这个值不影响存档位置。

## Shader precompile (without it the game stutters)

The emulator translates the game's shaders and compiles them into a form the graphics card can run.
When a scene, effect or enemy appears for the first time and its shaders are not ready yet, the game
**stutters for a fraction of a second to a few seconds**, most noticeably when entering a new area or
seeing a new effect for the first time. What you have already played is remembered and does not
stutter again.

There are two levels of preparation, both specific to the graphics card and driver:

1. **Background shader preparation (automatic)**: after every launch, the whole game's shaders are
   translated in the background while the top right corner shows the progress. It takes less than
   half a minute on the test PC and a few minutes on PCs with fewer threads, with a high CPU load
   meanwhile. The first launch makes the input files it needs.
2. **Full precompile (recommended once)**: compiles every shader and pipeline of the game into
   `_PipelineCache`. Afterwards shader stutters are essentially gone. With a recent NVIDIA driver the
   result is a store the game reads pipeline by pipeline (`<title>_<version>.binaries`, about 2 GB, nothing to
   load at launch); with other drivers a cache each launch loads (with a progress bar).
   - Choose "Precompile first" in the dialog at launch, or double-click `precompile.cmd` on its own.
   - The time depends on the CPU: under an hour with 22 threads, about 1 hour 50 minutes on a 16-thread
     laptop (Ryzen 9 5900HX), longer with fewer threads. The CPU is fully loaded meanwhile: running it
     overnight is a good idea. Once "ready to play" shows (about halfway), what play needs is in place
     and the game can be started; the rest follows.
   - Closing the window stops it; running it again continues where it stopped.
   - If the graphics driver never finishes compiling a pipeline (reported on an AMD card), the precompile leaves
     that pipeline out after 5 minutes and carries on; at the end it lists them in the logs folder
     (`stuck-pipelines.txt`). Please update the graphics driver, and if it happens again, report that file on GitHub.
   - Coming from an older package with a precompiled `_PipelineCache\static\<title>.bin`: the precompile
     turns it into the store in a few minutes; the `.bin` is then unused and can be deleted (about 4 GB).
   - It compiles the shaders as the game uses them in play too: the package carries what was recorded
     while playing every world on the test PC (`tools\local\static-precompile\hints-<title>_<version>.hints`,
     without the game's shader code), together with what your own play recorded, completed with the code of
     your own game files. For those shaders it leaves out the guessed variants the game did not use (a fifth
     of the time); `precompile.cmd -Full` compiles them too.
   - Places and effects the test PC never recorded (bosses' deaths, for one) still compile when they first
     appear. The launcher's "Export shader hints" button (or `precompile.cmd -ExportHints`) writes what your
     own play recorded, without the game's shader code, to `my-hints-<title>_<version>.hints.zip` (a few hundred
     KB, nothing is compiled) and shows it in Explorer: attached to a GitHub issue, it goes into the hints of the
     next packages.
   - **After a graphics driver update or a new graphics card it has to be done again**; the launcher
     asks again. It also asks after an update of the emulator that precompiles more: running it again
     then compiles only what is new.
   - To stop being asked: tick "Don't ask about precompiling again" (delete
     `no-precompile-prompt.txt` to be asked again).

## Controls

A gamepad is recommended (DualSense, DualShock 4 and Xbox controllers work; just plug them in).

Default keyboard layout:

| Key | Maps to |
| --- | --- |
| W A S D | Left stick (move) |
| T F G H | Right stick (camera: T up, G down, F left, H right) |
| J / L / K / I | × / ○ / □ / △ |
| Q / E | L1 / R1 |
| Left Shift / Left Ctrl | L3 / R3 |
| Arrow keys | D-pad |
| Enter | OPTIONS |
| Backspace / Tab | Left / right half of the touch pad |
| F7 | Camera with the mouse (press again to release the mouse) |
| Space, Pause | **Pause / resume the emulation** (careful not to hit Space by accident; once your own keys give Space to the game, only Pause) |
| F8 | The emulator's overlay: Off, Frame rate, Detailed (the launcher's "Overlay" is where it starts) |
| F9 | **Debug snapshot**: pauses the game and saves the picture on screen and where you are (`_DebugSnapshots\<date-time>\`: `screen.png`, `info.txt` with the position, the run's log and the switches); F9 again resumes. Attach the folder to an issue when something looks wrong |
| F11, Alt+Enter | Toggle full screen |

The keyboard has no L2 / R2 by default: use a gamepad when you need them.

### Your own keys

Add `"--keymap", "<control>=<key>"` pairs to the `"command"` list in `launch.json` (after `"kyty_emulator.exe"`).
With any of them, only your own keys work on the keyboard (none of the layout above):

```json
"--keymap", "LeftStickUp=W",
"--keymap", "Circle=Space",
"--keymap", "L1=Left Shift",
"--keymap", "R1=Mouse:Left",
"--keymap", "MouseSensitivity=1.5",
```

- Controls: `Cross Circle Square Triangle L1 R1 L2 R2 L3 R3 Options Up Down Left Right TouchPad TouchPadRight`,
  `LeftStickUp/Down/Left/Right`, `RightStickUp/Down/Left/Right`.
- Keys: their names on the keyboard (`E`, `Tab`, `Space`, `Escape`, `Left Shift`, `Left Ctrl`, `Return`, ...; also
  written without the space, as `LeftShift`), or `Mouse:Left`, `Mouse:Right`, `Mouse:Middle`, `Mouse:X1`, `Mouse:X2`.
- F7, F8, F9 and F11 stay the emulator's. A line it cannot use is skipped, and the run log says which and why
  (`Input mapping skipped: ...`).
- An update's `launch.json` replaces yours: keep a copy of your lines.

## Files and folders

| File / folder | Contents |
| --- | --- |
| `run.cmd` | Starts the game |
| `precompile.cmd` | Full shader precompile |
| `_SaveData` | **Saves** (created by the first run; back them up now and then). Each save the game makes is also kept in `_SaveData\.kyty-backup\<game>\<slot>\<date-time>` (the last 4, at least 5 minutes apart); a save left broken by a crash while saving (a file of zeros) gets the newest good one back when the game opens it, the broken files kept beside the backups, and the log says so |
| `_PipelineCache` | Shader caches (valid only for this PC's graphics card and driver: do not copy them to other PCs) |
| `logs` | Run logs |
| `game-path.txt` | The remembered game location |
| `seeds-<title>_<version>.seeds` | The list of shaders the first launch collects from the game files (one per game version) (it contains the game's shader code: do not share it), for the precompile |
| `hinted-<title>_<version>.seeds` | The recorded play of the package's hints and of your own play with the code of your game files (made by each precompile, from `released-<title>_<version>.seeds` and `recorded-play-<title>_<version>.hints`; the `.seeds` files contain the game's shader code: do not share them) |
| `launch.json` | The emulator's command line and extra switches; normally left alone |
| `launcher-settings.json` | The launcher's settings |
| `_DebugSnapshots` | The F9 debug snapshots (picture, position, log), to attach to an issue |
| `Content\<title>`, `dlc_emu.ini` | Add-on content (DLC), if you add it (see "Add-on content") |

## Updating to a newer package

Close the game and the launcher, then extract the new zip over the old folder and let it replace the
files. The zip holds only the program files (the emulator, its DLLs, the scripts, `launch.json`, this
README, the licenses and the shader hints under `tools`). Your saves, shader caches, logs and settings
are in files it does not have (`_SaveData`, `_PipelineCache`, `logs`, `game-path.txt`,
`launcher-settings.json`, the `seeds-...` files), so they stay as they are. The launcher asks for a new
precompile only when the new version needs one, and that run compiles only what is new. If you edited
`launch.json` yourself, make the edit again in the new one. A copy of `_SaveData` before an update never
hurts.

## Known issues

- It crashes now and then.
- On some PCs the graphics card hangs ("device lost", the screen goes black and the driver restarts, or a
  VIDEO_TDR_FAILURE blue screen), in a laptop RTX 3080's case within minutes at Boletaria: turn on "Safe GPU
  mode" in the launcher (see above). The frame rate is then about 37% lower.
- Sound: 3D sound has no direction, and there is no reverb. On a busy PC it may crackle: the sound buffer
  (20 ms) grows by itself after each dropout; to start with a bigger one: `run.cmd -Set KYTY_AUDIO_BUFFER_MS=60`
  (up to 200). Where the CPU is at its limit and the game makes its sound too late, the sound is stretched a
  little to fill the gaps instead of cutting out (`-Set KYTY_AUDIO_STRETCH=0` turns that off).
- It uses a lot of memory and video memory: on PCs with too little it may just exit.
- The MEMORY_MANAGEMENT blue screen (0x1A, 0x41202) that PCs whose RAM ran full got after 10 minutes to an hour
  of play (issue #50, mostly 16 GB with a browser open) is fixed: Windows mishandles no-access pages of a mapped
  file when it pages them out, and the emulator now keeps such pages from reads with guard pages instead
  (`run.cmd -Set KYTY_READ_GUARD=0` goes back to no access, for comparison). If you still get this blue screen,
  please report it in issue #50. A full RAM still makes the game slow: close other large programs while playing
  and keep a page file of at least 32 GB.
- On some TVs the picture tears or stutters with VSync on. A user's fix on a 60 Hz TV with an NVIDIA
  card (issue #30): in the NVIDIA Control Panel, Manage 3D settings > Program Settings, add
  `kyty_emulator.exe` and set "Vertical sync" to On and "Vulkan/OpenGL present method" to "Prefer native".
- With DLSS frame generation on, the game may stop at start-up with "Could not create the Vulkan device with DLSS
  frame generation". A user's fix on an RTX 5080 (issue #45): in Windows Security, App & browser control > Exploit
  protection > Program settings, add `kyty_emulator.exe` and, under "Randomize memory allocations (Bottom-up ASLR)",
  tick "Override system settings" and turn it On.
- Windows may say "Windows protected your PC" (the programs are not signed): click
  "More info → Run anyway". For a zip downloaded from the internet, you can tick "Unblock" in the
  zip file's properties before extracting it.

## When something goes wrong

Send the newest `.out.log` and `.err.log` from the `logs` folder to whoever gave you this package,
and describe where you were and what you were doing when it happened.
