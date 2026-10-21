# tools/sync-upstream.ps1
# Automated, safety-guarded sync workflow to update KytyPS5 from upstream chenxiao07/KytyPS5
# Preserves RTX 3070 DLSS, 4K UI/HUD canvas, and OpenDLSSG Frame Generation.

param(
    [Parameter(Position=0)]
    [ValidateSet('Check', 'StageCandidate', 'Promote')]
    [string]$Action = 'Check',

    [Parameter(Position=1)]
    [string]$TargetTag = '',

    [switch]$SkipBuild,
    [switch]$DryRun
)

$ErrorActionPreference = 'Stop'

# Path Resolution
$repoRoot = (Resolve-Path -LiteralPath (Split-Path $PSScriptRoot -Parent)).Path
$gameRoot = (Resolve-Path -LiteralPath (Split-Path $repoRoot -Parent)).Path
$buildDir = Join-Path -Path $repoRoot -ChildPath '_Build\windows'

# Locate glslangValidator
$localGlslang = Join-Path -Path $gameRoot -ChildPath 'tools\glslang\bin'
if (Test-Path -LiteralPath (Join-Path -Path $localGlslang -ChildPath 'glslangValidator.exe')) {
    if ($env:PATH -notlike "*$localGlslang*") {
        $env:PATH = "$localGlslang;$env:PATH"
    }
}

function Get-BaselineTag {
    $playbook = Join-Path -Path $repoRoot -ChildPath 'docs\UPSTREAM_UPDATE_PLAYBOOK.md'
    if (Test-Path -LiteralPath $playbook) {
        $content = Get-Content -LiteralPath $playbook -Raw
        if ($content -match 'Upstream:\s*\[chenxiao07/KytyPS5\s+(v[0-9A-Za-z._-]+)\]') {
            return $matches[1]
        }
    }
    return 'unknown'
}

function Get-UpstreamLatest {
    try {
        $ghRelease = & gh api repos/chenxiao07/KytyPS5/releases/latest --jq '.tag_name' 2>$null
        if ($LASTEXITCODE -eq 0 -and -not [string]::IsNullOrWhiteSpace($ghRelease)) {
            return $ghRelease.Trim()
        }
    } catch {}

    # Fallback to git ls-remote
    $tags = & git ls-remote --tags origin 'v202*' | ForEach-Object {
        if ($_ -match 'refs/tags/(v[0-9A-Za-z._-]+)$') { $matches[1] }
    } | Sort-Object -Descending
    if ($tags.Count -gt 0) { return $tags[0] }
    throw 'Failed to query upstream releases or tags.'
}

function Create-SafetyBackup {
    $timestamp = Get-Date -Format 'yyyyMMdd-HHmmss'
    $backupDir = Join-Path -Path $gameRoot -ChildPath "_Backup_UpstreamPromotion_$timestamp"
    Write-Host "[SAFETY] Creating pre-flight backup at: $backupDir" -ForegroundColor Cyan

    if ($DryRun) {
        Write-Host "[DRY-RUN] Would create backup directory $backupDir and archive active binaries/configs." -ForegroundColor Yellow
        return $backupDir
    }

    New-Item -ItemType Directory -LiteralPath $backupDir -Force | Out-Null
    $itemsToPreserve = @(
        'kyty_emulator.exe',
        'kyty_shader_precompile.exe',
        'launcher.cmd',
        'launcher.ps1',
        'launcher-settings.json',
        'launch.json',
        'version.dll'
    )
    foreach ($item in $itemsToPreserve) {
        $src = Join-Path -Path $gameRoot -ChildPath $item
        if (Test-Path -LiteralPath $src) {
            Copy-Item -LiteralPath $src -Destination $backupDir -Force
        }
    }

    $streamlineDir = Join-Path -Path $gameRoot -ChildPath 'streamline'
    if (Test-Path -LiteralPath $streamlineDir) {
        $destStreamline = Join-Path -Path $backupDir -ChildPath 'streamline'
        Copy-Item -LiteralPath $streamlineDir -Destination $destStreamline -Recurse -Force
    }

    $saveDataDir = Join-Path -Path $gameRoot -ChildPath '_SaveData'
    if (Test-Path -LiteralPath $saveDataDir) {
        $destSaveData = Join-Path -Path $backupDir -ChildPath 'SaveData-pre-merge'
        Copy-Item -LiteralPath $saveDataDir -Destination $destSaveData -Recurse -Force
    }

    # Generate SHA256 manifest of preserved files
    $manifest = Join-Path -Path $backupDir -ChildPath 'SHA256SUMS.txt'
    Get-ChildItem -LiteralPath $backupDir -Recurse -File | ForEach-Object {
        $rel = $_.FullName.Substring($backupDir.Length + 1).Replace('\', '/')
        '{0}  {1}' -f (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant(), $rel
    } | Set-Content -LiteralPath $manifest -Encoding ascii

    Write-Host "[SAFETY] Backup completed successfully." -ForegroundColor Green
    return $backupDir
}

# --- Action: Check ---
if ($Action -eq 'Check') {
    $currentBaseline = Get-BaselineTag
    $latestUpstream = Get-UpstreamLatest

    Write-Host "==================================================" -ForegroundColor Cyan
    Write-Host " KytyPS5 Upstream Sync Check" -ForegroundColor Cyan
    Write-Host "==================================================" -ForegroundColor Cyan
    Write-Host "Current Fork Baseline : $currentBaseline"
    Write-Host "Latest Upstream Tag   : $latestUpstream"

    if ($currentBaseline -eq $latestUpstream) {
        Write-Host "Status: UP TO DATE. Current fork is aligned with upstream $latestUpstream." -ForegroundColor Green
    } else {
        Write-Host "Status: NEW UPDATE AVAILABLE ($latestUpstream)!" -ForegroundColor Yellow
        Write-Host "To stage a candidate build, run:"
        Write-Host "  .\tools\sync-upstream.ps1 -Action StageCandidate -TargetTag $latestUpstream" -ForegroundColor Cyan
    }
    exit 0
}

# --- Action: StageCandidate ---
if ($Action -eq 'StageCandidate') {
    if ([string]::IsNullOrWhiteSpace($TargetTag)) {
        $TargetTag = Get-UpstreamLatest
    }

    Write-Host "==================================================" -ForegroundColor Cyan
    Write-Host " Staging Candidate Build for Upstream $TargetTag" -ForegroundColor Cyan
    Write-Host "==================================================" -ForegroundColor Cyan

    Push-Location $repoRoot
    try {
        # Check working tree
        $status = & git status --porcelain
        if ($status) {
            if ($DryRun) {
                Write-Host "[WARN] Working tree is dirty, but proceeding with DryRun..." -ForegroundColor Yellow
            } else {
                throw "Git working tree is not clean. Commit or stash existing changes before staging."
            }
        }

        # Backup
        $backupDir = Create-SafetyBackup

        # Create branch
        $branchName = "test/upstream-$TargetTag-merge"
        Write-Host "[GIT] Fetching upstream tag $TargetTag..." -ForegroundColor Cyan
        if (-not $DryRun) {
            & git fetch origin "refs/tags/$TargetTag`:refs/tags/$TargetTag" --force
            if ($LASTEXITCODE -ne 0) { throw "Failed to fetch upstream tag $TargetTag." }

            $existingBranch = & git branch --list $branchName
            if ($existingBranch) {
                Write-Host "[GIT] Branch $branchName already exists, checking out..." -ForegroundColor Yellow
                & git checkout $branchName
            } else {
                Write-Host "[GIT] Creating branch $branchName from feature/rtx3070-dlssg..." -ForegroundColor Cyan
                & git checkout feature/rtx3070-dlssg
                & git checkout -b $branchName
            }

            Write-Host "[GIT] Merging upstream tag $TargetTag..." -ForegroundColor Cyan
            & git merge $TargetTag --no-commit
            if ($LASTEXITCODE -ne 0) {
                Write-Host "[WARN] Automatic merge encountered conflicts." -ForegroundColor Yellow
                Write-Host "Please inspect and resolve conflicts preserving RTX 3070 integrations." -ForegroundColor Yellow
                Write-Host "Core files to verify:"
                Write-Host "  - src/local/hud-anchor.h (InterfaceScope & 4K UI canvas)"
                Write-Host "  - src/graphics/host_gpu/renderer/cache/textureCache.cpp (DXGI VRAM pressure)"
                Write-Host "  - src/graphics/host_gpu/vma.cpp (64 MiB block sizing)"
                exit 1
            }
        }

        # Build Candidate
        if (-not $SkipBuild) {
            Write-Host "[BUILD] Cleaning build directory to prevent ThinLTO PGO profile mismatch..." -ForegroundColor Cyan
            if (-not $DryRun) {
                if (Test-Path -LiteralPath $buildDir) {
                    cmake --build $buildDir --target clean 2>$null
                }
                $env:KYTY_CMAKE_ARGS = "-DKYTY_MARCH=x86-64-v3"
                cmd /c "build-windows.cmd kyty_emulator"
                if ($LASTEXITCODE -ne 0) { throw "Emulator compilation failed." }
                cmd /c "build-windows.cmd kyty_shader_precompile"
                if ($LASTEXITCODE -ne 0) { throw "Shader precompile compilation failed." }
                cmd /c "build-windows.cmd all_tests"
                if ($LASTEXITCODE -ne 0) { throw "Test suite compilation failed." }
            }

            # Run tests
            Write-Host "[TEST] Running CTest regression suite..." -ForegroundColor Cyan
            if (-not $DryRun) {
                ctest --test-dir $buildDir -C Release --output-on-failure
                if ($LASTEXITCODE -ne 0) { throw "CTest regression tests failed." }

                Write-Host "[TEST] Running Launcher Command validation..." -ForegroundColor Cyan
                & powershell -NoProfile -ExecutionPolicy Bypass -File (Join-Path -Path $repoRoot -ChildPath 'tests\LauncherCommandTests.ps1')
                if ($LASTEXITCODE -ne 0) { throw "Launcher command tests failed." }
            }
        }

        # Deploy Candidate to Game Root
        $candEmuName = "kyty_emulator_candidate_$TargetTag.exe"
        $candPreName = "kyty_shader_precompile_candidate_$TargetTag.exe"
        $candEmuDest = Join-Path -Path $gameRoot -ChildPath $candEmuName
        $candPreDest = Join-Path -Path $gameRoot -ChildPath $candPreName

        Write-Host "[DEPLOY] Deploying candidate binaries to game root beside version.dll..." -ForegroundColor Cyan
        if (-not $DryRun) {
            Copy-Item -LiteralPath (Join-Path -Path $buildDir -ChildPath 'kyty_emulator.exe') -Destination $candEmuDest -Force
            Copy-Item -LiteralPath (Join-Path -Path $buildDir -ChildPath 'kyty_shader_precompile.exe') -Destination $candPreDest -Force

            # Create candidate test launcher
            $testLauncherCmd = Join-Path -Path $gameRoot -ChildPath "launcher-test-upstream-$TargetTag.cmd"
            @("@echo off",
              "setlocal",
              "title Demon's Souls - Upstream $TargetTag Candidate Test Launcher",
              "cd /d `"%~dp0`"",
              "`"%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe`" -NoProfile -ExecutionPolicy Bypass -File `"%~dp0launcher.ps1`" -EmulatorExecutable `"%~dp0$candEmuName`" %*",
              "if %errorlevel% neq 0 pause"
            ) | Set-Content -LiteralPath $testLauncherCmd -Encoding ascii
        }

        Write-Host ""
        Write-Host "==================================================" -ForegroundColor Green
        Write-Host " Candidate Successfully Staged for Verification! " -ForegroundColor Green
        Write-Host "==================================================" -ForegroundColor Green
        Write-Host "Candidate Executable : $candEmuDest"
        Write-Host "Candidate Launcher   : launcher-test-upstream-$TargetTag.cmd"
        Write-Host ""
        Write-Host "NEXT STEPS FOR MANUAL VERIFICATION:" -ForegroundColor Yellow
        Write-Host "1. Launch: Run launcher-test-upstream-$TargetTag.cmd"
        Write-Host "2. Verify 1080p DLSS -> 1440p output & 4K UI/HUD in-game"
        Write-Host "3. Verify OpenDLSSG 2x Frame Generation & VRAM stability"
        Write-Host "4. Check save game loading and progression"
        Write-Host "5. When satisfied, promote candidate to default by running:"
        Write-Host "   .\tools\sync-upstream.ps1 -Action Promote -TargetTag $TargetTag" -ForegroundColor Cyan
    } finally {
        Pop-Location
    }
    exit 0
}

# --- Action: Promote ---
if ($Action -eq 'Promote') {
    if ([string]::IsNullOrWhiteSpace($TargetTag)) {
        throw "TargetTag is mandatory for Promote action (e.g. -TargetTag v20261020b)."
    }

    Write-Host "==================================================" -ForegroundColor Cyan
    Write-Host " Promoting Upstream $TargetTag to Default & Release" -ForegroundColor Cyan
    Write-Host "==================================================" -ForegroundColor Cyan

    Push-Location $repoRoot
    try {
        $candEmuName = "kyty_emulator_candidate_$TargetTag.exe"
        $candPreName = "kyty_shader_precompile_candidate_$TargetTag.exe"
        $candEmu = Join-Path -Path $gameRoot -ChildPath $candEmuName
        $candPre = Join-Path -Path $gameRoot -ChildPath $candPreName

        if (-not (Test-Path -LiteralPath $candEmu) -or -not (Test-Path -LiteralPath $candPre)) {
            throw "Candidate binaries not found in game root: $candEmu"
        }

        $emuHash = (Get-FileHash -LiteralPath $candEmu -Algorithm SHA256).Hash.ToLowerInvariant()
        $preHash = (Get-FileHash -LiteralPath $candPre -Algorithm SHA256).Hash.ToLowerInvariant()

        Write-Host "[PROMOTION] Promoting candidate binaries to default in game root..." -ForegroundColor Cyan
        if (-not $DryRun) {
            Copy-Item -LiteralPath $candEmu -Destination (Join-Path -Path $gameRoot -ChildPath 'kyty_emulator.exe') -Force
            Copy-Item -LiteralPath $candPre -Destination (Join-Path -Path $gameRoot -ChildPath 'kyty_shader_precompile.exe') -Force
        }

        # Dry-run default launcher
        Write-Host "[VERIFY] Running launcher dry run..." -ForegroundColor Cyan
        if (-not $DryRun) {
            & powershell -NoProfile -ExecutionPolicy Bypass -File (Join-Path -Path $gameRoot -ChildPath 'launcher.ps1') -DryRun | Out-Null
            if ($LASTEXITCODE -ne 0) { throw "Default launcher dry-run failed." }
        }

        # Resolve commits
        $releaseCommit = (& git rev-parse HEAD).Trim()
        $binarySourceCommit = (& git rev-parse "HEAD~1^{commit}").Trim()

        $releaseTag = "$TargetTag-upstream-merge"
        if ($TargetTag -match '^v\d{8}') {
            # Format v20261020b -> v2026.10.20b-upstream-merge
            $datePart = $TargetTag.Substring(1, 4) + '.' + $TargetTag.Substring(5, 2) + '.' + $TargetTag.Substring(7)
            $releaseTag = "v$datePart-upstream-merge"
        }

        Write-Host "[GIT] Fast-forwarding feature/rtx3070-dlssg and fix/dlss-output-ui-canvas..." -ForegroundColor Cyan
        if (-not $DryRun) {
            & git checkout feature/rtx3070-dlssg
            & git merge --ff-only $releaseCommit
            & git checkout fix/dlss-output-ui-canvas
            & git merge --ff-only $releaseCommit
            & git tag -a $releaseTag -m "Release $($releaseTag): Upstream $TargetTag with RTX 3070 DLSS, 4K UI canvas and Frame Gen"
        }

        Write-Host "[PACKAGE] Packaging Windows release zip and manifest..." -ForegroundColor Cyan
        if (-not $DryRun) {
            & (Join-Path -Path $PSScriptRoot -ChildPath 'package-windows-release.ps1') `
                -Tag $releaseTag `
                -BinarySourceCommit $binarySourceCommit `
                -EmulatorSha256 $emuHash `
                -PrecompileSha256 $preHash
            if ($LASTEXITCODE -ne 0) { throw "Release packaging failed." }
        }

        Write-Host "[GIT] Pushing branches and release tag to myfork..." -ForegroundColor Cyan
        if (-not $DryRun) {
            & git push myfork feature/rtx3070-dlssg fix/dlss-output-ui-canvas $releaseTag
            if ($LASTEXITCODE -ne 0) { throw "Failed to push branches/tags to myfork." }
        }

        Write-Host "[GITHUB] Publishing release via gh release create..." -ForegroundColor Cyan
        $zipPath = Join-Path -Path $repoRoot -ChildPath "_Build\package\$releaseTag\KytyPS5-$releaseTag-Windows-x64.zip"
        $sumsPath = Join-Path -Path $repoRoot -ChildPath "_Build\package\$releaseTag\SHA256SUMS.txt"
        $notesPath = Join-Path -Path $repoRoot -ChildPath "docs\RELEASE-NOTES.md"

        if (-not $DryRun) {
            & gh release create $releaseTag $zipPath $sumsPath `
                --repo giangero1/KytyPS5-RTX30-FrameGen `
                --title "KytyPS5 $($releaseTag): RTX 3070 DLSS + Frame Gen + Upstream $TargetTag" `
                --notes-file $notesPath `
                --latest
            if ($LASTEXITCODE -ne 0) { throw "Failed to publish GitHub release." }
        }

        Write-Host ""
        Write-Host "==================================================" -ForegroundColor Green
        Write-Host " Promotion and Release Completed Successfully!   " -ForegroundColor Green
        Write-Host "==================================================" -ForegroundColor Green
        Write-Host "Release Tag : $releaseTag"
        Write-Host "GitHub URL  : https://github.com/giangero1/KytyPS5-RTX30-FrameGen/releases/tag/$releaseTag"
    } finally {
        Pop-Location
    }
    exit 0
}
