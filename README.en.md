# Aerofly FS 4 × Rokid Flight HUD

## ⬇ Download APK & DLL

### [English APK · v2.3.1](https://github.com/sjkxciuciu/aerofly-rokid-hud/raw/refs/heads/main/release/AeroflyRokidHud-2.3.1-English.apk)

### [中文 APK · v2.3.0](https://github.com/sjkxciuciu/aerofly-rokid-hud/raw/refs/heads/main/release/AeroflyRokidHud-debug.apk)

### [Windows DLL · v2.3.0](https://github.com/sjkxciuciu/aerofly-rokid-hud/raw/refs/heads/main/release/AeroflyRokidHud.dll)

Choose one APK for the glasses and download the DLL for the PC. These links download the files directly. [Checksums](release/SHA256SUMS.txt).

---

[简体中文](README.md) | **English**

Display Aerofly FS 4 flight instruments on display-equipped Rokid glasses.

**Landscape dashboard · Large green HUD · Chinese alerts · Live telemetry over LAN**

This project includes a Windows C++ external DLL and an Android APK. The PC reads simulator data and sends it to the glasses over the local network. Normal operation does not require a cloud service or a phone relay.

This is an unofficial personal project, with no official affiliation with IPACS or Rokid. The latest downloads are **Chinese APK 2.3.0, English APK 2.3.1, and the 2.3.0 DLL**. The overview and previews below were originally prepared for experimental version 2.1.0. Builds, local protocol tests, and layout previews have been checked; on-glasses viewing and real Wi-Fi switching have not yet been verified.

**Language note:** a separate English APK is available in the download section above. Choose the APK for your preferred language.

![Landscape HUD layout preview](docs/HUD-wide-live.png)

The image uses simulated values and was generated from the actual HudView drawing code through a desktop Canvas adapter. **It is not a screenshot captured from the glasses.**

## Features

| Area | Instruments |
| --- | --- |
| Left | Indicated airspeed in kt, altitude in ft, vertical speed in ft/min |
| Center | Heading, pitch/bank attitude indicator, overall landing gear position |
| Right | Segmented flap and spoiler indicators with percentages |
| Top | Flight/connection status and Chinese alerts |
| Bottom | Throttle percentage, transmission mode, packet rate, time since last packet, retry and malformed-packet counts |

- A 640×480 landscape design with large text on a green-on-black display. A 480×640 portrait layout is retained for firmware that still supplies a portrait window.
- Chinese alerts for engine fire, master warning, low oil/fuel/hydraulic pressure, altitude alert, and caution.
- Landing gear states: up, in transit, down, or unavailable. This is overall position, not independent downlock indication for each gear leg.
- Stale instruments show “--”. Missing data is not treated as zero or gear up.
- Automatic PC discovery followed by unicast delivery, unicast registration keepalives to the learned PC, receiver retries, and rediscovery after a timeout.
- A DLL heartbeat independent of flight-data updates, allowing the HUD to distinguish a responding PC with stale flight data from a silent sender.

**Reading the display:** ENG / 油门 uses Aircraft.Throttle. It is throttle percentage, not actual engine thrust or N1. “Time since last packet” is measured locally on the glasses and is not round-trip network latency. When no heartbeat is received, the protocol alone cannot determine whether the game was closed or the network failed.

## Download and installation

Current build artifacts are included in the repository:

- [Android APK](release/AeroflyRokidHud-debug.apk)
- [Windows x64 DLL](release/AeroflyRokidHud.dll)
- [SHA-256 checksums](release/SHA256SUMS.txt)

If GitHub opens a binary preview page, use **Download raw file**.

### PC

1. Close Aerofly FS 4 completely.
2. Place the DLL in this location under your Windows Documents folder:

   ```text
   Aerofly FS 4\external_dll\AeroflyRokidHud.dll
   ```

   Create external_dll if it does not exist. This is the Documents location, not the Steam installation directory.
3. Restart the game and enter a flight.

### Glasses

The target is **display-equipped Rokid glasses that allow installation of a compatible Android APK**. The APK requires Android 7.0 / API 24 or later. Installation permissions and landscape support depend on the model and firmware and still require device testing.

Enable developer mode and ADB on the device, then install:

```powershell
adb install -r .\release\AeroflyRokidHud-debug.apk
```

Open **Aerofly HUD** and connect the PC and glasses to the same local network. “实时飞行” means valid flight data is being received.

The supplied APK is signed with a development debug key. The repository does not include the private signing key. A build produced on another computer will normally have a different signature and cannot directly replace an installed APK signed with the original key.

## Network and troubleshooting

| Port | Purpose |
| --- | --- |
| UDP 49002 | Telemetry reception on the glasses |
| UDP 49003 | Discovery and registration reception on the PC |

- Windows Firewall must allow Aerofly on the trusted private network being used.
- The same Wi-Fi name does not guarantee device-to-device communication. Guest networks, client isolation, or broadcast filtering can prevent initial discovery.
- If discovery fails, try connecting the glasses to a PC hotspot.
- After registration, the PC sends unicast data and retains low-frequency broadcasts. Recovery time depends on the devices and network; there is no fixed reconnection-time guarantee.
- The UDP protocol has no authentication or encryption. Use it only on a trusted local network.

| On-screen message | Meaning |
| --- | --- |
| 实时飞行 | Live flight: PC communication and the main flight fields are valid |
| 等待游戏 | Waiting for flight data: PC heartbeat is present, but main fields are missing or stale |
| 寻找电脑 / 正在重连 | Searching for PC / reconnecting: no data yet, or recent packets have timed out |
| 不可用 / -- | Unavailable: the field has no valid fresh value |
| 测试数据 | Simulated test data |

To test the HUD with simulated telemetry, close Aerofly first and run:

```powershell
.\Test-Hud-Stream.ps1 -Seconds 60
# Optionally specify the glasses' IP address:
.\Test-Hud-Stream.ps1 -Target 192.168.1.50 -Seconds 60
```

Simulated packets are labeled “测试数据” on screen. The script uses the same discovery port as the DLL; do not run them simultaneously.

## Building from source

### DLL

Requires Windows, Visual Studio 2022 Build Tools, MSVC v143, and a Windows SDK:

```powershell
.\Build-Plugin.ps1
```

The script downloads the header from the official IPACS External DLL Sample, builds the x64 DLL, places it in release, and copies it into the current user's Aerofly external DLL folder under Documents. **Close the game before running it.**

### APK

Requires a working JDK (JDK 21 was used locally) and network access for build tools:

```powershell
.\Build-Apk.ps1
```

The script downloads the Android SDK and Gradle into .build-cache and produces release/AeroflyRokidHud-debug.apk. The current toolchain uses Gradle 8.10.2, Android Gradle Plugin 8.8.2, and compile/target SDK 35.

The existing script automatically accepts Android SDK licenses. Review and agree to the relevant licenses before using it.

## Project structure

```text
aerofly-plugin/        Read-only C++ telemetry bridge and official SDK header
glasses-app/           Java Android receiver, protocol parser, and Canvas HUD
docs/                 Layout previews
release/              Current APK, DLL, and checksums
Build-Plugin.ps1       Build and install the PC DLL
Build-Apk.ps1          Build the APK
Test-Hud-Stream.ps1    Simulated telemetry sender
```

## Validation status

Completed: C++ compilation, APK build and signature verification, local DLL loading and UDP unicast tests, unit conversion, stale-data handling, re-registration, and reinitialization; Java receiver checks for malformed and out-of-order packets, 30 rapid pause/resume cycles, rebinding after silence, and recovery after a port becomes available; desktop previews for normal, warning, and disconnected states.

Not yet completed: on-glasses viewing, real Wi-Fi switching, or verification of every aircraft's fields and alert behavior. Fields not provided by the simulator are shown as unavailable.

## Versions and third-party material

- [Changelog](CHANGELOG.en.md)
- [2.1.0 release notes — Chinese and English](RELEASE_NOTES.md)
- [Third-party notices](THIRD_PARTY_NOTICES.md)
- [IPACS developer page and External DLL Sample](https://www.aerofly.com/developer/)

Aircraft.VerticalSpeed is supplied in m/s and converted to ft/min. Aircraft.Gear uses 0 for up, 1 for down, and intermediate values for transit. The project reads simulator data and does not send control commands to the game.
