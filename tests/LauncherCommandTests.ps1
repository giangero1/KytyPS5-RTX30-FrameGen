param([string]$EmulatorExe = (Join-Path $PSScriptRoot '../_Build/windows/kyty_emulator.exe'))
$ErrorActionPreference = 'Stop'
$launcher = Join-Path $PSScriptRoot '../launcher.ps1'
$ast = [Management.Automation.Language.Parser]::ParseFile($launcher, [ref]$null, [ref]$null)
$function = $ast.Find({ param($n) $n -is [Management.Automation.Language.FunctionDefinitionAst] -and $n.Name -eq 'Get-PlayCommand' }, $true)
Invoke-Expression $function.Extent.Text
$root = Split-Path (Split-Path $launcher -Parent) -Parent
$powershellCommand = 'powershell'
$EmulatorExe = [IO.Path]::GetFullPath($EmulatorExe)
$streamline = 'C:\runtime with spaces\streamline'
$frameGenAvailable = $true
foreach ($dlss in @($false, $true)) {
    foreach ($fg in @(0, 2)) {
        $settings = @{ resolution='2560x1440'; language=1; fullscreen=$true; aspect=$true; redzone=$true; fps120=$true;
            vsync=$true; hdr=$false; upscaler=$(if ($dlss) {'dlss'} else {''}); render=1440; render_dlss=1080;
            ratio='16:9'; quality='standard'; framegen=$fg; hud=1; safegpu=$false; game='C:\game with spaces' }
        $command = Get-PlayCommand
        if (!$command.Contains('-Exe "'+$EmulatorExe+'"')) { throw 'Wrong executable' }
        if ($command.Contains('-Upscaler dlss') -ne $dlss) { throw 'Wrong upscaler' }
        if ($command.Contains('-RenderHeight 1080') -ne $dlss) { throw 'Wrong render height' }
        if ($command.Contains('-FrameGen 1') -ne ($fg -eq 2)) { throw 'Wrong frame generation multiplier' }
        if (!$command.Contains('-Width 2560 -Height 1440')) { throw 'Wrong output dimensions' }
        Write-Output "PASS dlss=$dlss framegen=$fg executable=$EmulatorExe"
    }
}
