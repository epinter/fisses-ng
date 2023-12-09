param([string]$buildPreset="ALL-release", [string]$configPreset="ALL", [string]$packConfig, [string]$packPreset)
$ErrorActionPreference = "Stop"
Set-StrictMode -Version 3.0

Write-Warning "Using buildPreset='$buildPreset' and configPreset='$configPreset'"

$projectRoot = Resolve-Path "$PSScriptRoot/.."

if(!$packPreset -and !$packConfig) {
    $packConfig=$buildPreset.split("-")[1]
}

Set-Location $projectRoot
Write-Warning "Starting cmake"
cmake -B build -S "$projectRoot" --preset $configPreset --fresh

Write-Warning "Starting cmake --build with preset '$($buildPreset)'"
cmake --build "$projectRoot/build" --preset "$($buildPreset)" --install
Set-Location "$projectRoot/build"

if($packPreset) {
    Write-Warning "Starting cpack with preset '$($packPreset)'"
    cpack --preset "$($packPreset)"
} elseif($packConfig) {
    Write-Warning "Starting cpack with config '$($packConfig)'"
    cpack -C "$($packConfig)"
}

Set-Location $projectRoot
