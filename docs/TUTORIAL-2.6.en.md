# v2.6 AutoRunway tutorial

[Downloads](../README.en.md) · [简体中文](TUTORIAL-2.6.zh-CN.md)

## Install and upgrade

1. Download and extract the **2.6-AutoRunway-Bilingual** ZIP linked on the homepage, not the original v2.6 bundle.
2. Close the game. Press Windows + R and enter `shell:Personal`. Put the new **AeroflyRokidHud.dll and AeroflyRokidRunways.dat** together in `Aerofly FS 4\external_dll` under Documents. Create that folder if needed; do not use the Steam directory.
3. Install one language APK:
   `adb install -r AeroflyRokidHud-2.6-AutoRunway-English.apk`
   Use the Chinese file for Chinese. Approve USB debugging if ADB reports unauthorized. Both languages use the same application ID and replace each other.
4. Keep CMD and PS1 in the same extracted folder and launch **Set-ReferenceRunway.cmd**.
5. Click **Automatic runway selection** to enable it explicitly. Development notes saying it was enabled on the developer's PC do not mean your PC is configured.
6. Join a reachable LAN on both devices, restart the simulator and enter a flight, then open the HUD.

Updating only the APK is insufficient. These APKs use a debug signature; a differently signed build may not replace an existing installation. Consider app data before uninstalling.

## Controls and automatic selection

Tap to cycle HUD → map → 3° reference. Swipe forward/backward on the map to zoom. Online tiles need internet; telemetry and runway selection do not.

Candidates are sought about 0.1–20 NM before the threshold, within 45° of the approach heading and near the runway extension. A candidate must stay stable for about 3 seconds; near the centerline within 8 NM, it is locked to reduce switching. Lost telemetry, airport changes or significant departure from the approach clear or reselect it.

Always check the airport/runway label, especially for parallel runways. This is geometry-based selection, not the FMS or ATC clearance. The database is not comprehensive and may differ from scenery.

## Manual runway / disable / automatic

Open the CMD, fill these fields and click **Save / Enable** for manual mode:

| Field | Value |
| --- | --- |
| Runway name | 1–24 letters, numbers, spaces, / or - |
| Threshold latitude | Landing-threshold decimal degrees, north positive |
| Threshold longitude | Same threshold, east positive |
| Threshold elevation | Feet MSL, not aircraft altitude |
| Landing direction | TRUE course along the runway from the landing threshold, 0–360°, not magnetic |

Use the correct scenery threshold, not airport-center coordinates. Do not infer true course from the runway number. Use decimal points. Configuration: `Documents/Aerofly FS 4/external_dll/AeroflyRokidApproach.ini`.

**Automatic runway selection** restores auto mode; **Disable reference** turns it off. The settings tool indicates roughly one second for reload; confirm on the HUD. Replacing the DLL still requires a game restart.

## Troubleshooting

| Symptom | Check |
| --- | --- |
| CMD cannot find script | Extract fully; keep PS1 alongside CMD |
| Missing database | Place DAT beside the new DLL in external_dll |
| Reference unavailable | Auto mode, DAT, fresh telemetry, approach region and candidate stability |
| Wrong parallel runway | Check label and use manual mode |
| Searching for PC | Game restart, DLL location, LAN reachability, firewall, guest isolation |
| Map absent but instruments work | Internet connectivity; tiles and LAN telemetry use different connections |

PC receives UDP 49003; glasses receive UDP 49002. Do not disable the whole firewall or lower global script policy. Follow managed-computer rules.

## Readings and validation

The reference assumes 3° and 50 ft threshold crossing using barometric altitude. Set QNH correctly. Positive deviation means aircraft above reference, with the diamond below center. Distance is along the runway extension, not DME slant range. Stale or invalid data is not valid guidance.

Not ILS, with no terrain or obstacle protection. Simulation only. Missing G is unavailable, not zero.

The author confirmed historical v2.5 device testing. New AutoRunway development notes record local tests, not on-glasses validation. Historical validation does not establish compatibility for all new features, firmware or aircraft.
