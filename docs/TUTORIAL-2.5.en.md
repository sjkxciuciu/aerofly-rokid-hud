# 2.5 installation and runway-reference tutorial

[简体中文](TUTORIAL-2.5.zh-CN.md) · [Home](../README.en.md)

## 1. Download and extract

[Download the complete bilingual 2.5 bundle](https://github.com/sjkxciuciu/aerofly-rokid-hud/raw/refs/heads/main/release/AeroflyRokidHud-2.5-Bilingual.zip)

Extract the ZIP first. It includes both APKs, the DLL, CMD launcher, PS1 settings tool, and notes. **Keep Set-ReferenceRunway.cmd and Set-ReferenceRunway.ps1 in the same folder. Do not run them from inside the ZIP.**

## 2. Update both components

1. Close Aerofly FS 4 completely.
2. Press Windows + R and enter `shell:Personal`. Copy the matching DLL into `Aerofly FS 4\external_dll` under Documents, not the Steam installation folder.
3. Install either APK on the glasses. Both language versions share an application ID and version number, so one replaces the other.

With ADB, run from the extracted folder:

```powershell
adb devices
adb install -r .\AeroflyRokidHud-2.5-English.apk
# For Chinese, use:
# adb install -r .\AeroflyRokidHud-2.5-Chinese.apk
```

The device must show status `device`. Approve USB debugging on the glasses if it shows `unauthorized`. Version 2.5 requires the matching DLL as well as the APK.

## 3. Connect

Join the same reachable LAN on both devices, enter a flight in Aerofly, and open the HUD. Allow Aerofly on the trusted private network: the PC receives UDP 49003 and the glasses receive UDP 49002. Initial discovery requires broadcasts to pass.

Check that instruments change with the simulator. An unavailable glide reference before runway configuration is normal.

## 4. Configure the reference runway

Double-click **Set-ReferenceRunway.cmd** to open the settings window.

| Field | Value |
| --- | --- |
| Runway name | A label such as ICAO / 09; letters, numbers, spaces, / or - |
| Threshold latitude | Landing-threshold latitude in decimal degrees; north positive |
| Threshold longitude | The same threshold's longitude in decimal degrees; east positive |
| Threshold elevation | Threshold elevation in feet MSL, not aircraft altitude |
| Landing direction | TRUE course from the landing threshold along the runway; 0–360°, not magnetic course |

Use data matching your simulator scenery. Do not enter airport-center coordinates or infer true course from the runway number. Use a decimal point.

Click **Save / Enable**. The plugin reloads the settings periodically; the tool indicates about one second. Confirm the runway label on the glasses. The configuration is saved to:

```text
Documents/Aerofly FS 4/external_dll/AeroflyRokidApproach.ini
```

Update it manually when changing destination; it does not follow the FMS. **Disable reference** turns it off.

## 5. Controls and readings

- Tap the glasses' touchpad to cycle HUD → map → glide reference → HUD.
- Swipe forward/backward on the map to zoom in/out.
- Positive vertical deviation means the aircraft is above the reference. The diamond represents the reference relative to the aircraft, so it appears below center when the aircraft is high.
- Distance is along the runway extension to the threshold, not DME slant range.

Actual touch-event forwarding depends on the glasses' firmware and still needs device confirmation. The layout remains horizontally arranged.

## 6. Troubleshooting

| Symptom | Check |
| --- | --- |
| CMD cannot find its script | Extract the ZIP and keep the PS1 next to the CMD |
| Plugin directory not found | Install the DLL and create external_dll first |
| Save fails | Check numeric ranges, decimal points, and runway-name characters |
| Instruments work but reference is unavailable | Enable the configuration, check runway data and aircraft position within the reference region |
| PC cannot be found | DLL location, game restart, LAN reachability, firewall, and guest isolation |

Do not disable the whole firewall or lower the global PowerShell policy. Follow administrator rules on managed computers.

## Reference limitations

This is a **3° geometric reference for simulation, not ILS or a flight director**. It assumes a 50 ft threshold-crossing height and uses simulator barometric altitude. Set QNH and threshold elevation correctly. It provides no real glide-slope signal, terrain protection, or obstacle protection.

The reference is limited to 0.1–20 NM before the threshold, approximately ±60° approach direction, and near the runway extension. The lateral region is ±10°, with a minimum half-width of 0.1 NM. The diamond is hidden when unconfigured, missing/stale data, past the threshold, or outside the region. Do not use it for real flight.

See [local test notes](../release/2.5-tests.txt). Desktop checks are not on-glasses validation.
