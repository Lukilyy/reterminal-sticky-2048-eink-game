# reTerminal Sticky — 2048 E-Ink Game

A compact portrait 2048 game running on Seeed Studio reTerminal Sticky, featuring a 3.97" 800×480 E-Ink display.

It uses the native 1-bit E-paper framebuffer and ESP-IDF directly, without
LVGL, networking, cloud services, or a filesystem.

## Features

- 480 x 800 portrait UI on the SSD1677 E-paper display
- GT911 swipe input with an independent 20 ms polling task
- Partial refresh during play and periodic full refresh cleanup
- AI button: short press starts a new game; long press powers off
- BQ27220 battery percentage and GPIO9 external-power indication
- BQ25616 charging enabled through GPIO39
- NVS persistence for Best Score and the current game
- Optional LEDC buzzer interface

## Requirements

- reTerminal E1005 / Sticky hardware
- ESP-IDF 5.4
- ESP32-S3 target with PSRAM

## Build and flash

```bash
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

Replace `PORT` with the device serial port. Project defaults live in
`sdkconfig.defaults`; the generated `sdkconfig` is intentionally ignored.

## Controls

- Swipe up, down, left, or right to move the board.
- Tap `NEW GAME`, or short-press the AI button, to reset the board.
- Long-press the AI button to save the current game and power off.

## Project structure

```text
main/
├── game/       # Pure C++ 2048 logic and separate NVS persistence
├── hardware/   # Display, touch, battery, AI button, and buzzer adapters
├── ui/         # 1-bit framebuffer game view and bitmap font
├── main.cpp    # Application wiring and refresh policy
└── pin_config.h

components/
├── bq27220/
├── button/
├── debug_logging/
├── gt911/
└── seeed_epaper/
```

`debug_logging` is retained because the GT911 and E-paper drivers depend on
it. The `button` component is vendored locally so the project does not require
generated managed components.

## GPIO summary

| Function | GPIO |
|---|---:|
| AI / power button | 4 |
| External power detect | 9 |
| E-paper SPI | 13-18 |
| GT911 I2C | 2, 3 |
| GT911 enable / interrupt / reset | 42, 21, 41 |
| BQ27220 sensor I2C | 0, 1 |
| Charger enable, active low | 39 |
| Buzzer PWM | 48 |
| Power hold / lock | 45, 46 |

## Scope

This repository intentionally excludes Wi-Fi, BLE, OTA, MQTT, cloud
communication, RTC screens, LVGL, animations, and the original peripheral
test programs. It keeps only the drivers and application code required by the
Sticky 2048 firmware.
