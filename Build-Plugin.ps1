$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$pluginRoot = Join-Path $projectRoot 'aerofly-plugin'
$sdkDir = Join-Path $pluginRoot 'sdk'
$zip = Join-Path $env:TEMP 'aerofly_external_dll_sample_20250605.zip'
$sample = Join-Path $env:TEMP 'aerofly_external_dll_sample_20250605'

New-Item -ItemType Directory -Force -Path $sdkDir, $sample | Out-Null
Invoke-WebRequest 'https://dl1.aerofly.com/aerofly_fs_4_project_external_dll_sample_20250605.zip' -OutFile $zip
Expand-Archive -LiteralPath $zip -DestinationPath $sample -Force
$header = Get-ChildItem -Path $sample -Filter tm_external_message.h -Recurse | Select-Object -First 1
if ($null -eq $header) { throw 'IPACS SDK header was not found in the downloaded sample.' }
Copy-Item -LiteralPath $header.FullName -Destination (Join-Path $sdkDir 'tm_external_message.h') -Force

$vswhere = 'C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe'
$msbuild = $null
if (Test-Path $vswhere) {
  $installRoot = & $vswhere -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
  if ($installRoot) {
    $candidate = Join-Path $installRoot 'MSBuild\Current\Bin\amd64\MSBuild.exe'
    if (Test-Path $candidate) { $msbuild = Get-Item $candidate }
  }
}
if ($null -eq $msbuild) { throw 'Visual Studio 2022 Build Tools (Desktop development with C++) was not found.' }
& $msbuild.FullName (Join-Path $pluginRoot 'AeroflyRokidHud.vcxproj') /p:Configuration=Release /p:Platform=x64 /m
if ($LASTEXITCODE -ne 0) { throw 'DLL build failed.' }

$destination = Join-Path ([Environment]::GetFolderPath('MyDocuments')) 'Aerofly FS 4\external_dll'
$release = Join-Path $projectRoot 'release'
New-Item -ItemType Directory -Force -Path $destination | Out-Null
New-Item -ItemType Directory -Force -Path $release | Out-Null
$builtDll = Join-Path $pluginRoot 'build\AeroflyRokidHud.dll'
Copy-Item $builtDll $release -Force
Copy-Item $builtDll $destination -Force
Write-Host "Installed: $destination\AeroflyRokidHud.dll"
Write-Host "Release: $release\AeroflyRokidHud.dll"
