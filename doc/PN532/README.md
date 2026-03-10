# PN532 Web Serial 调试页

一个纯前端网页工具，用浏览器直接通过 Web Serial 与 PN532 通信（115200 / 8N1），支持十六进制收发、扩展命令、自动轮询寻卡和响应解析。

## 功能

- 串口连接/断开（Web Serial）
- 手动发送 HEX 数据（空格分隔）
- 实时接收并显示 RX HEX
- 实时解析 PN532 接收帧（ACK / LEN+DCS 校验 / 响应码解释）
- 常见响应可读化解释（固件信息、寻卡 UID、InDataExchange 状态/读块数据）
- 快捷命令按钮（按功能分组）
- 自动轮询寻卡（可配置周期）
- Ultralight 快捷读页(0x30)/写页(0xA2)
- 命令序列发送（可配置间隔）

## 内置命令（新增）

- 设备类：`GetFirmwareVersion`、`GetGeneralStatus`、`ReadGPIO`、`SAMConfiguration`
- 目标管理：`InListPassiveTarget`、`InSelect`、`InRelease`
- 交换类：`InDataExchange`（用于读写卡）
- 诊断类：`Diagnose(0x00)`
- 初始化序列：唤醒 + SAM + 固件读取 + 寻卡

> 其中帧封装（LEN/LCS/DCS）由页面自动计算，手动发送区仍支持直接发完整原始帧。

## 使用方式

1. 用 Chrome 或 Edge（需支持 Web Serial）。
1. 在项目目录启动本地静态服务（推荐）：

```bash
cd /Users/chanyan/Code/PN532
python3 -m http.server 8080
```

1. 浏览器打开：

```text
http://localhost:8080
```

1. 点击“连接串口”，选择 PN532 所在串口。
1. 使用手动 HEX 发送或快捷命令按钮。

## 默认串口参数

- 波特率：`115200`
- 数据位：`8`
- 停止位：`1`
- 校验位：`none`
- 流控：`none`

## 注意事项

- Web Serial 需要安全上下文，建议 `localhost` 或 `https`。
- 串口底层按原始字节收发；快捷命令会自动封装 PN532 帧，手动发送区可直接发送任意原始 HEX 帧。

## UI 说明

- 左侧：连接、手动 HEX、扩展命令、卡片功能操作区
- 右侧：原始串口日志 + PN532 解析日志
- 顶部状态：连接状态、最近 UID、轮询开关状态
