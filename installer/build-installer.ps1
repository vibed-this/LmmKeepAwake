#Requires -Version 5.1
<#
.SYNOPSIS
    Build the LmmKeepAwake Inno Setup installer (x64, per-user).

.DESCRIPTION
    Expects the Release exe to already exist. Compiles
    installer/LmmKeepAwake.iss with ISCC.exe and writes a SHA256 file
    next to the Setup exe in dist/.

.EXAMPLE
    powershell -File installer/build-installer.ps1

.EXAMPLE
    powershell -File installer/build-installer.ps1 -Version 0.2.0
#>
[CmdletBinding()]
param(
    [string]$Version = "",
    [string]$ISCC = ""
)

$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Parent $PSScriptRoot
$iss = Join-Path $PSScriptRoot "LmmKeepAwake.iss"
$exe = Join-Path $repoRoot "build/windows-msvc-release/Release/LmmKeepAwake.exe"
$distDir = Join-Path $repoRoot "dist"

if (-not (Test-Path -LiteralPath $iss)) {
    throw "Installer script not found: $iss"
}
if (-not (Test-Path -LiteralPath $exe)) {
    throw ("Release exe not found: $exe. " +
        "Build it first: cmake --preset windows-msvc-release; " +
        "cmake --build --preset windows-msvc-release")
}

$candidates = @()
if (-not [string]::IsNullOrWhiteSpace($ISCC)) {
    $candidates += $ISCC
}
$isccCmd = Get-Command iscc -ErrorAction SilentlyContinue
if ($null -ne $isccCmd) {
    $candidates += $isccCmd.Source
}
$candidates += @(
    (Join-Path ${env:ProgramFiles(x86)} "Inno Setup 6\ISCC.exe"),
    (Join-Path $env:ProgramFiles "Inno Setup 6\ISCC.exe"),
    # winget default (per-user) install location
    (Join-Path $env:LOCALAPPDATA "Programs\Inno Setup 6\ISCC.exe")
)

$isccExe = $candidates | Where-Object { -not [string]::IsNullOrWhiteSpace($_) -and (Test-Path -LiteralPath $_) } | Select-Object -First 1
if ($null -eq $isccExe) {
    throw ("ISCC.exe not found. Install Inno Setup 6 from https://jrsoftware.org/isinfo.php " +
        "or pass -ISCC <path-to-ISCC.exe>.")
}

$isccArgs = @()
if (-not [string]::IsNullOrWhiteSpace($Version)) {
    $isccArgs += "/DMyAppVersion=$Version"
}
$isccArgs += $iss

Write-Host "Compiling installer with $isccExe"
& $isccExe @isccArgs
if ($LASTEXITCODE -ne 0) {
    throw "ISCC.exe failed with exit code $LASTEXITCODE."
}

$setup = Get-ChildItem -Path $distDir -Filter "LmmKeepAwake-*-Setup.exe" | Sort-Object LastWriteTime -Descending | Select-Object -First 1
if ($null -eq $setup) {
    throw "ISCC succeeded but no LmmKeepAwake-*-Setup.exe was found in $distDir."
}
(Get-FileHash -LiteralPath $setup.FullName -Algorithm SHA256).Hash | Out-File -FilePath "$($setup.FullName).sha256" -NoNewline -Encoding ascii
Write-Host "Installer: $($setup.FullName)"
Write-Host "SHA256   : $($setup.FullName).sha256"

