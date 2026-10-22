param(
    [Parameter(Mandatory=$true)][ValidatePattern('^v[0-9][A-Za-z0-9._-]*$')][string]$Tag,
    [Parameter(Mandatory=$true)][string]$BinarySourceCommit,
    [Parameter(Mandatory=$true)][ValidatePattern('^[a-fA-F0-9]{64}$')][string]$EmulatorSha256,
    [Parameter(Mandatory=$true)][ValidatePattern('^[a-fA-F0-9]{64}$')][string]$PrecompileSha256,
    [string]$BuildDirectory = '',
    [string]$OutputRoot = ''
)
$ErrorActionPreference = 'Stop'
$repo = Split-Path $PSScriptRoot -Parent
if (!$BuildDirectory) { $BuildDirectory = Join-Path $repo '_Build/windows' }
if (!$OutputRoot) { $OutputRoot = Join-Path $repo '_Build/package' }
$BuildDirectory = (Resolve-Path -LiteralPath $BuildDirectory).Path
$name = "KytyPS5-$Tag-Windows-x64"
$releaseDir = Join-Path $OutputRoot $Tag
if (Test-Path -LiteralPath $releaseDir) { throw "Release output already exists: $releaseDir" }
Push-Location $repo
try {
    $releaseCommit = & git rev-parse HEAD
    if ($LASTEXITCODE) { throw 'Cannot resolve release commit' }
    $binaryCommit = & git rev-parse "$BinarySourceCommit^{commit}"
    if ($LASTEXITCODE) { throw 'Cannot resolve binary source commit' }
    & git merge-base --is-ancestor $binaryCommit $releaseCommit
    if ($LASTEXITCODE) { throw 'Binary source is not an ancestor of this release' }
    $dirty = & git status --porcelain
    if ($LASTEXITCODE -or $dirty) { throw 'Commit all source/release changes before packaging' }
    # Documentation and packaging may follow the build; compiled inputs must not change.
    & git diff --quiet $binaryCommit $releaseCommit -- src 3rdparty CMakeLists.txt build-windows.cmd tools/pgo
    if ($LASTEXITCODE) { throw 'Compiled inputs changed since the verified build; rebuild and validate first' }
    $expected = @{'kyty_emulator.exe'=$EmulatorSha256; 'kyty_shader_precompile.exe'=$PrecompileSha256}
    foreach ($item in $expected.GetEnumerator()) {
        if ((Get-FileHash -LiteralPath (Join-Path $BuildDirectory $item.Key) -Algorithm SHA256).Hash -ne $item.Value) {
            throw "Unverified build artifact: $($item.Key)"
        }
    }
    $out = Join-Path $releaseDir $name
    New-Item -ItemType Directory -Path $out | Out-Null
    foreach ($file in @('kyty_emulator.exe','kyty_shader_precompile.exe','libwinpthread-1.dll')) {
        Copy-Item -LiteralPath (Join-Path $BuildDirectory $file) -Destination $out
    }
    $launcherExe = Join-Path $repo '_Build/launcher/KytyLauncher.exe'
    if (Test-Path -LiteralPath $launcherExe) {
        Copy-Item -LiteralPath $launcherExe -Destination $out
    }
    Copy-Item run-windows.ps1, precompile-windows.ps1, launcher.ps1, launcher.cmd, LICENSE -Destination $out
    Copy-Item LICENSES -Destination $out -Recurse
    Copy-Item run-windows.json -Destination "$out/launch.json"
    Copy-Item docs/PORTABLE-README.md -Destination "$out/README.md"
    New-Item -ItemType Directory -Path "$out/docs/images" | Out-Null
    Copy-Item docs/RTX30_FRAME_GEN_GUIDE.md, docs/DLSS_UI_CANVAS_VALIDATION.md, docs/UPSTREAM_UPDATE_PLAYBOOK.md, docs/RELEASE-NOTES.md -Destination "$out/docs"
    Copy-Item docs/images/rtx3070-*.png -Destination "$out/docs/images"
    $states = "$out/tools/local/static-precompile"
    New-Item -ItemType Directory -Path $states | Out-Null
    Copy-Item tools/local/static-precompile/pass-states.json, tools/local/static-precompile/hints-*.hints -Destination $states
    Get-ChildItem tools/local/static-precompile/files-*.files -File | Copy-Item -Destination $states
    $vs = & "${env:ProgramFiles(x86)}/Microsoft Visual Studio/Installer/vswhere.exe" -latest -products * -property installationPath
    if ($LASTEXITCODE -or !$vs) { throw 'Visual Studio installation not found for VC redistributables' }
    $crt = Get-Item "$vs/VC/Redist/MSVC/*/x64/Microsoft.VC14*.CRT" |
        Where-Object { $_.Parent.Parent.Name -match '^\d+\.' } |
        Sort-Object { [version]$_.Parent.Parent.Name } | Select-Object -Last 1
    if (!$crt) { throw 'VC runtime redist not found' }
    Copy-Item "$($crt.FullName)/msvcp140*.dll", "$($crt.FullName)/vcruntime140*.dll" -Destination $out
    Set-Content "$out/run.cmd" -Encoding ASCII -Value @('@echo off', '"%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe" -NoProfile -ExecutionPolicy Bypass -File "%~dp0run-windows.ps1" -Prompt -Follow %*', 'if %errorlevel% neq 0 pause')
    Set-Content "$out/precompile.cmd" -Encoding ASCII -Value @('@echo off', '"%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe" -NoProfile -ExecutionPolicy Bypass -File "%~dp0precompile-windows.ps1" %*', 'pause')
    @("binary_source_commit=$binaryCommit", "release_commit=$releaseCommit", "tag=$Tag",
      'configuration=Release', 'cpu=x86-64-v3 (AVX2)', 'compiler=clang-cl 19; ThinLTO; Streamline headers enabled',
      "emulator_sha256=$($EmulatorSha256.ToLowerInvariant())", "precompile_sha256=$($PrecompileSha256.ToLowerInvariant())",
      'build=locally built and gameplay-verified; binaries not rebuilt for packaging',
      'NVIDIA Streamline runtime and OpenDLSSG proxy DLLs are separate prerequisites.') |
        Set-Content "$out/BUILD-INFO.txt" -Encoding utf8
    if (@(Get-ChildItem "$out/docs/images/*.png").Count -ne 6) { throw 'Expected six screenshots' }
    foreach ($dll in @('libwinpthread-1.dll','msvcp140.dll','vcruntime140.dll')) {
        if (!(Test-Path -LiteralPath "$out/$dll")) { throw "Missing runtime: $dll" }
    }
    foreach ($item in $expected.GetEnumerator()) {
        if ((Get-FileHash -LiteralPath "$out/$($item.Key)").Hash -ne $item.Value) { throw 'Packaged executable mismatch' }
    }
    & powershell -NoProfile -ExecutionPolicy Bypass -File "$out/launcher.ps1" -DryRun |
        Set-Content "$releaseDir/launcher-dry-run.log" -Encoding utf8
    if ($LASTEXITCODE) { throw 'Packaged launcher failed' }
    $payload = @(Get-ChildItem -LiteralPath $out -Recurse -File | Sort-Object FullName)
    $payload | ForEach-Object {
        '{0}  {1}' -f (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant(), $_.FullName.Substring($out.Length+1).Replace('\','/')
    } | Set-Content "$out/SHA256-MANIFEST.txt" -Encoding ascii
    $zip = Join-Path $releaseDir "$name.zip"
    Compress-Archive -LiteralPath $out -DestinationPath $zip -CompressionLevel Optimal
    $zipHash = (Get-FileHash -LiteralPath $zip -Algorithm SHA256).Hash.ToLowerInvariant()
    "$zipHash  $name.zip" | Set-Content "$releaseDir/SHA256SUMS.txt" -Encoding ascii
    Write-Output "ZIP=$zip"
    Write-Output "SHA256=$zipHash"
} finally { Pop-Location }
