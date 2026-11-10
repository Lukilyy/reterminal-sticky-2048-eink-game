# SEEED_EPAPER

`SEEED_EPAPER` 是一个面向 ESP-IDF 的模块化墨水屏组件，当前先支持：

- `UC8179`：reTerminal E1001
- `SSD1677`：reTerminal E1005
- `SSD2677`：reTerminal Sticky 新版屏幕

SSD 系列面板使用 `seeed_epaper_new_panel(SEEED_EPAPER_PANEL_SSD1677, ...)`
入口自动识别：发送 `0x70` 读取 ID，`0x07` 选择 SSD2677，否则选择 SSD1677。
SPI 设备需配置 `SPI_DEVICE_HALFDUPLEX | SPI_DEVICE_3WIRE`，通过双向 MOSI 读回。
本组件以本地官方 `Seeed-reTerminal-Sticky-Firmware` 为基线，
保留 SSD1677 全刷参数 `0xF4`。本 Demo 的适配差异为先安装 GPIO ISR 服务
再注册 BUSY 中断，并在普通日志级别输出探测到的原始 ID。

设计目标：

- SPI 总线由外部创建，组件只接收 `spi_device_handle_t`
- `panel` 核心与 `driver` 控制器实现分层，后续加新屏只需补 driver
- 对外统一支持整屏刷新、任意区域局刷、1bpp 原生输入和 4/8/16bit 转 1bpp
- 为后续灰度 / 彩色扩展预留能力描述与像素格式枚举

## 对外 API

- `seeed_epaper_new_panel()`：创建面板句柄
- `seeed_epaper_panel_prepare()`：准备一次 full / partial 会话
- `seeed_epaper_panel_write_bitmap()`：写入一块区域到显存
- `seeed_epaper_panel_commit()`：触发物理刷新
- `seeed_epaper_panel_refresh_area()`：单次区域写入并刷新
- `seeed_epaper_panel_sleep()`：进入休眠

推荐给 LVGL9 的接法：

1. `flush_cb` 中对每个 chunk 调 `prepare + write_bitmap`
2. `lv_display_flush_is_last(display)` 为真时再 `commit`
3. 首帧走 `FULL`，后续走 `PARTIAL`
4. `LV_COLOR_DEPTH=1` 时直接传 `MONO1_MSB`
5. `LV_COLOR_DEPTH=8/16` 时传 `GRAY8` / `RGB565`，组件内部阈值转换为 1bpp

## 目录结构

- `epaper_panel.h`：统一公共接口
- `epaper_panel.c`：GPIO / SPI / 像素转换 / 会话管理
- `driver/uc8179.c`：UC8179 控制器实现
- `driver/ssd1677.c`：SSD1677 控制器实现
- `driver/ssd2677.c`：SSD2677 控制器实现

