$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $MyInvocation.MyCommand.Path
$app = Join-Path $root 'glasses-app'
$cache = Join-Path $root '.build-cache'
$gradleVersion = '8.10.2'
$gradleZip = Join-Path $cache "gradle-$gradleVersion-bin.zip"
$gradleHome = Join-Path $cache "gradle-$gradleVersion"
$sdk = Join-Path $cache 'android-sdk'
New-Item -ItemType Directory -Force -Path $cache, $sdk | Out-Null
if (!(Test-Path $gradleHome)) {
  Invoke-WebRequest "https://services.gradle.org/distributions/gradle-$gradleVersion-bin.zip" -OutFile $gradleZip
  Expand-Archive -LiteralPath $gradleZip -DestinationPath $cache -Force
}
if (!(Test-Path (Join-Path $sdk 'cmdline-tools\latest\bin\sdkmanager.bat'))) {
  $toolsZip = Join-Path $cache 'commandlinetools-win-11076708_latest.zip'
  $toolsTemp = Join-Path $cache 'cmdline-tools-tmp'
  Invoke-WebRequest 'https://dl.google.com/android/repository/commandlinetools-win-11076708_latest.zip' -OutFile $toolsZip
  Expand-Archive -LiteralPath $toolsZip -DestinationPath $toolsTemp -Force
  New-Item -ItemType Directory -Force -Path (Join-Path $sdk 'cmdline-tools\latest') | Out-Null
  Move-Item (Join-Path $toolsTemp 'cmdline-tools\*') (Join-Path $sdk 'cmdline-tools\latest') -Force
}
$sdkmanager = Join-Path $sdk 'cmdline-tools\latest\bin\sdkmanager.bat'
# sdkmanager asks more than once; provide one answer per prompt through PowerShell's pipeline.
1..20 | ForEach-Object { 'y' } | & $sdkmanager "--sdk_root=$sdk" --licenses | Out-Null
& $sdkmanager "--sdk_root=$sdk" 'platform-tools' 'platforms;android-35' 'build-tools;35.0.0'
$env:ANDROID_HOME = $sdk
& (Join-Path $gradleHome 'bin\gradle.bat') -p $app assembleDebug
if ($LASTEXITCODE -ne 0) { throw 'APK build failed.' }
$release = Join-Path $root 'release'
New-Item -ItemType Directory -Force -Path $release | Out-Null
Copy-Item (Join-Path $app 'app\build\outputs\apk\debug\app-debug.apk') (Join-Path $release 'AeroflyRokidHud-debug.apk') -Force
Write-Host "Created: $release\AeroflyRokidHud-debug.apk"
