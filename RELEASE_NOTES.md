# v2.1.0 | Landscape Flight HUD · 横向大字飞行仪表

[简体中文](#简体中文) | [English](#english)

## 简体中文

这是一套面向 Aerofly FS 4 和带显示屏 Rokid 眼镜的个人项目：Windows 外部 DLL 读取游戏数据，Android HUD 在局域网内接收并显示。

本版采用横向宽屏大字布局，包含空速、高度、垂直速度、航向、水平仪、起落架、襟翼、减速板、油门比例和中文警报。连接面板显示传输模式、有效收包频率及数据新鲜度。

### 下载内容

- AeroflyRokidHud-debug.apk：Android 眼镜端，版本 2.1.0。
- AeroflyRokidHud.dll：Windows x64 电脑端，使用 2.0 数据桥接协议实现。
- SHA256SUMS.txt：安装包校验值。

### 安装

首次使用时，将 DLL 放入文档目录的 Aerofly FS 4/external_dll，然后在眼镜上安装 APK。电脑与眼镜连接同一可信局域网，打开 HUD 并进入游戏飞行。

已经使用 2.0/2.0.1 版本时，只需覆盖安装本版 APK，DLL 无需更换。

### 当前状态

本版为实验版本，已通过构建、签名校验、本机数据传输与恢复测试；尚未完成眼镜实机佩戴及真实 Wi-Fi 切换验证。横屏支持取决于眼镜固件；预览图不是实拍截图。

油门百分比不等于实际推力或 N1；缺失的机型字段显示不可用。下载后的完整安装说明见 README。

## English

A personal project connecting Aerofly FS 4 to display-equipped Rokid glasses: a Windows external DLL reads simulator telemetry, and an Android HUD receives it over the local network.

This release introduces a landscape dashboard with large text. It displays airspeed, altitude, vertical speed, heading, attitude, overall landing gear position, flaps, spoilers, throttle percentage, and Chinese alerts. The connection panel shows transmission mode, valid packet rate, and data freshness.

The English documentation is included; the APK's status messages and alerts remain in Chinese.

### Downloads

- AeroflyRokidHud-debug.apk: Android glasses app, version 2.1.0.
- AeroflyRokidHud.dll: Windows x64 bridge using the existing v2 telemetry protocol.
- SHA256SUMS.txt: artifact checksums.

### Installation

For a first installation, place the DLL in Aerofly FS 4/external_dll under your Windows Documents folder, then install the APK on the glasses. Connect both devices to the same trusted local network, open the HUD, and enter a flight.

If you already use version 2.0 or 2.0.1, update only the APK. The DLL does not need to be replaced.

### Current status

This is an experimental release. Builds, signature verification, and local telemetry/recovery tests have passed. On-glasses viewing and real Wi-Fi switching have not yet been verified. Landscape support depends on the glasses' firmware; preview images are not device screenshots.

Throttle percentage is not actual thrust or N1. Aircraft fields that are not provided are shown as unavailable. See [the English README](README.en.md) for full setup instructions.
