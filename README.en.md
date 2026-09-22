# Aerofly FS 4 × Rokid Flight HUD

[简体中文](README.md)

## ⬇ Latest v2.6 AutoRunway (versionCode 29)

### [📦 下载完整包 / Download complete bundle](https://github.com/sjkxciuciu/aerofly-rokid-hud/raw/refs/heads/main/release/AeroflyRokidHud-2.6-AutoRunway-Bilingual.zip)

[English APK](https://github.com/sjkxciuciu/aerofly-rokid-hud/raw/refs/heads/main/release/AeroflyRokidHud-2.6-AutoRunway-English.apk) · [中文 APK](https://github.com/sjkxciuciu/aerofly-rokid-hud/raw/refs/heads/main/release/AeroflyRokidHud-2.6-AutoRunway-Chinese.apk) · [DLL](https://github.com/sjkxciuciu/aerofly-rokid-hud/raw/refs/heads/main/release/AeroflyRokidHud.dll) · [Required runway database](https://github.com/sjkxciuciu/aerofly-rokid-hud/raw/refs/heads/main/release/AeroflyRokidRunways.dat)

**Backup settings tool:** [CMD launcher](https://github.com/sjkxciuciu/aerofly-rokid-hud/raw/refs/heads/main/release/Set-ReferenceRunway.cmd) + [Required PS1](https://github.com/sjkxciuciu/aerofly-rokid-hud/raw/refs/heads/main/release/Set-ReferenceRunway.ps1) — keep together.
**Flight logs:** [Open the PC log folder (CMD)](release/Open-FlightRecords.cmd) · [Recorder notes](release/2.6-FlightRecorder说明.txt). Logs are saved on the PC under external_dll/flight-records as CSV at about 1 Hz. Update to the DLL in the complete bundle, then start a flight to record automatically.

Once automatic mode is enabled, you do not need to open this tool for normal use. Use it only to enable automatic mode initially, manually override an incorrect runway selection, or disable/restore the reference.

### [📖 Installation and automatic-runway tutorial](docs/TUTORIAL-2.6.en.md)

**Update the APK and DLL, and install the runway database.** The original v2.6 / G DLL does not provide AutoRunway. Choose one language APK; they replace each other.

[Checksums](release/SHA256SUMS.txt) · [Development notes and validation scope](release/2.6-AutoRunway说明.txt)

## Features and limitations

- Wide, large green HUD: speed, altitude, vertical speed, attitude, heading, gear, flaps, spoilers, alerts, connection diagnostics and throttle percentage.
- Persistent G readings on HUD, map and reference pages; missing data remains unavailable.
- Online basemap and flight track. LAN telemetry and automatic runway selection work without internet; online map tiles require internet.
- Runway selection uses position, true heading and threshold geometry, not the FMS or ATC assignment. Always check the label, especially near parallel runways.
- The reference assumes 3° and a 50 ft threshold crossing: **not ILS**. Set QNH correctly. Simulation only.
- Throttle percentage is not actual thrust or N1. Overall gear position is not individual downlock confirmation.

## Quick installation

1. Close Aerofly FS 4 and extract the complete bundle.
2. Press Windows + R, enter `shell:Personal`, and put **AeroflyRokidHud.dll and AeroflyRokidRunways.dat** in `Aerofly FS 4\external_dll` under Documents, not the Steam folder.
3. Install one language APK on the glasses (Android 7.0 / API 24 or newer):
   `adb install -r AeroflyRokidHud-2.6-AutoRunway-English.apk`
4. Run the CMD and click **Automatic runway selection**. Downloading the bundle does not configure your PC automatically.
5. Connect both devices to a reachable LAN, restart the simulator and enter a flight, then open the HUD. Tap to cycle HUD → map → reference; swipe forward/backward on the map to zoom.

Keep CMD and PS1 together. Existing settings may retain manual mode. Save / Enable selects a manual runway; Disable reference turns the reference off.

## Network and troubleshooting

The PC receives UDP 49003; glasses receive UDP 49002. Allow Aerofly on your trusted private network. Guest networks, client isolation and broadcast filtering can prevent discovery. Do not disable the whole firewall. UDP is unauthenticated and unencrypted: trusted LANs only.

No PC: check DLL location, game restart and network. No automatic reference: check DAT, automatic mode, fresh telemetry and approach position. Candidates must remain stable for about 3 seconds; no reference outside the approach region is normal. See the tutorial.

## Validation and preview

**The author confirmed device testing for v2.5, not this new v2.6 AutoRunway update.** New-version notes record local DLL/UDP, selection/locking, regression and Android build/lint checks, but no on-glasses validation of the new features. This upload checks APK version/signatures and bundle consistency, not every device or aircraft.

![v2.6 preview of all three pages](docs/2.6-all-pages.png)

Rendered from the current v2.6 screen layout: instruments, map, and automatic runway reference. Values are illustrative, not glasses photos; map roads are schematic, while live basemap tiles load for the aircraft's position.

## Source and older versions

Existing repository source, build scripts and old release notes remain for reference. **They have not been synchronized with these new binaries and are not guaranteed to reproduce this release.** Use the latest bundle and v2.6 tutorial above; older APK/ZIP files are historical artifacts.

[Historical changelog](CHANGELOG.en.md) · [Historical v2.5 tutorial](docs/TUTORIAL-2.5.en.md) · [Third-party notices](THIRD_PARTY_NOTICES.md)

Unofficial personal project, not affiliated with IPACS or Rokid. Runway data comes from [OurAirports](https://ourairports.com/data/) and may differ from simulator scenery. Online maps use OpenStreetMap. Simulation only, never real flight.
