# 2.5 安装与跑道参考设置教程

[English](TUTORIAL-2.5.en.md) · [返回首页](../README.md)

## 1. 下载并解压整包

[下载 2.5 中英文整包](https://github.com/sjkxciuciu/aerofly-rokid-hud/raw/refs/heads/main/release/AeroflyRokidHud-2.5-Bilingual.zip)

解压后包含中文 APK、英文 APK、DLL、CMD、PS1 和说明文件。**CMD 与同名 PS1 必须保存在同一个文件夹内，不要在 ZIP 内直接运行。**

## 2. 更新电脑 DLL 和眼镜 APK

1. 完全退出 Aerofly FS 4。
2. 按 Windows + R，输入 `shell:Personal` 打开“文档”，将配套 `AeroflyRokidHud.dll` 放进 `Aerofly FS 4\external_dll`。不要放入 Steam 游戏目录。
3. 在眼镜安装中文或英文 APK，二选一。两个包使用相同应用 ID 和版本号，切换语言会覆盖安装。

若使用 ADB，在解压目录运行：

```powershell
adb devices
adb install -r .\AeroflyRokidHud-2.5-Chinese.apk
# 选择英文时改用下面这条：
# adb install -r .\AeroflyRokidHud-2.5-English.apk
```

设备状态需为 `device`；出现 `unauthorized` 时先在眼镜确认 USB 调试授权。本版必须同时更新 APK 和 DLL。

## 3. 连接游戏

电脑和眼镜连接同一个可互通的局域网，启动游戏进入飞行，再打开眼镜 HUD。允许 Aerofly 在可信专用网络通信：电脑接收 UDP 49003，眼镜接收 UDP 49002。首次发现需要网络允许广播。

确认仪表随游戏变化。尚未设置跑道时，下滑参考显示不可用是正常现象，不代表连接失败。

## 4. 双击 CMD 设置参考跑道

双击解压后的 **Set-ReferenceRunway.cmd**。它会启动同目录的 PowerShell 设置界面：

| 输入项 | 填什么 |
| --- | --- |
| Runway name | 机场/跑道名称，如 ICAO / 09；使用英文字母、数字、空格、/ 或 - |
| Threshold latitude | 着陆方向的跑道入口纬度，十进制度；北正南负 |
| Threshold longitude | 同一个入口的经度，十进制度；东正西负 |
| Threshold elevation | 入口标高，单位 ft MSL；不是飞机高度 |
| Landing direction | 从着陆入口沿跑道向另一端的真航向，0–360°；不是磁航向 |

使用与你游戏场景一致的数据；不要填写机场中心坐标，也不要根据跑道编号猜真航向。小数使用英文句点。

点击 **Save / Enable** 保存启用。插件会周期读取配置，设置工具提示约 1 秒生效；查看眼镜显示的跑道名称确认。配置写入：

```text
文档/Aerofly FS 4/external_dll/AeroflyRokidApproach.ini
```

换目的地时要重新设置，它不会自动跟随游戏航路。点击 **Disable reference** 可关闭参考。

## 5. 眼镜操作和读数

- 镜腿轻触：仪表 → 地图 → 下滑参考 → 仪表。
- 地图页向前滑放大，向后滑缩小。
- 垂直偏差为正：飞机高于参考线。菱形显示参考线相对飞机的位置，因此飞机偏高时菱形在中线下方。
- 距离是沿跑道延长线到入口的距离，不是 DME 斜距。

2.5 已经实机验证（作者确认）；其他型号或固件的触控兼容性可能不同。显示布局仍为左右展开。

## 6. 无法打开或没有参考时

| 现象 | 检查 |
| --- | --- |
| 双击 CMD 找不到文件 | 先完整解压，并确认 Set-ReferenceRunway.ps1 与 CMD 同目录 |
| 提示 Plugin directory not found | 先按第 2 步安装 DLL，建立 external_dll 文件夹 |
| 保存失败 | 检查输入范围、小数点和名称字符 |
| 仪表正常但参考不可用 | 配置是否启用，跑道是否正确，数据是否有效，飞机是否位于参考区域 |
| 始终寻找电脑 | DLL 位置、游戏是否重启、同网互通、防火墙和访客隔离 |

不要为运行工具关闭整个防火墙或全局降低 PowerShell 策略。单位电脑若阻止脚本运行，请遵循其管理规则。

## 参考功能的范围

这是**模拟游戏用的 3° 几何参考，不是 ILS 或飞行指引仪**。固定假设在入口上方 50 ft 通过，使用游戏气压高度；须正确设置 QNH 和入口标高。它没有真实下滑台信号、地形或障碍物保护。

只有入口前 0.1–20 NM、进近方向约 ±60°、延长线附近才显示参考；横向范围为 ±10°、最小半宽 0.1 NM。未配置、缺少/过期数据、越过入口或离开区域时隐藏菱形。不要用于真实飞行。

本地测试记录见 [2.5-tests.txt](../release/2.5-tests.txt)。另据作者确认，2.5 已经在 Rokid 眼镜上实机验证；该测试记录保留原有电脑端测试范围。
