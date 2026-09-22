@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "$ErrorActionPreference='Stop'; $d=Join-Path ([Environment]::GetFolderPath('MyDocuments')) 'Aerofly FS 4\external_dll\flight-records'; New-Item -ItemType Directory -Force -Path $d | Out-Null; if (@(Get-ChildItem -LiteralPath $d -Filter '*.csv' -ErrorAction SilentlyContinue).Count -eq 0) { Write-Host 'No flight logs yet. Start a flight with the updated DLL first.' }; Invoke-Item -LiteralPath $d"
if errorlevel 1 pause
