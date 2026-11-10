# reTerminal Sticky 2048 E-Ink Game

A community 2048 game demo running on Seeed Studio reTerminal Sticky.

This project demonstrates native ESP-IDF development on reTerminal Sticky,
including E-Ink rendering, touch interaction, partial refresh, and hardware
integration.

The game uses the native 1-bit E-paper framebuffer and ESP-IDF directly,
without LVGL, networking, cloud services, or filesystem dependencies.

<p align="center">
  <img src="images/2048.jpg" alt="reTerminal Sticky running 2048" width="50%">
</p>

## Features

### Version 2.0.0

- One firmware supports automatic SSD1677 / SSD2677 controller detection.
- Shared-SPI startup supports display operation with a MicroSD card inserted.
- The AI button saves the game and enters Deep Sleep, then wakes and restores
  the game on the next press, including while USB power is connected.
- Touch polling stops before peripheral power is disabled, preventing I2C
  errors during sleep entry.

Both controller revisions were tested on physical devices by the maintainer.

- Native ESP-IDF implementation
- Portrait 480 × 800 UI with automatic SSD1677 / SSD2677 E-Ink controller detection
- 2048 game logic implemented in C++
- GT911 touch swipe control with an independent polling task
- Partial refresh during gameplay with periodic full refresh cleanup
- AI button support:
  - Short press: start a new game
  - Long press: save the current game state and enter Deep Sleep
- BQ27220 battery percentage display
- External power detection
- NVS persistence for:
  - Best score
  - Current game state
- Optional LEDC buzzer interface

## Hardware Requirements

- reTerminal Sticky
- ESP32-S3 with PSRAM
- 3.97-inch 800 × 480 E-Ink display

## Development Environment

- ESP-IDF v5.4+
- Python 3.x

## Build and Flash

Set the target, build the firmware, then flash and open the serial monitor:

```bash
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

Replace `PORT` with the serial port of your reTerminal Sticky.

Project configuration is provided through `sdkconfig.defaults`. The generated
`sdkconfig` file is intentionally excluded from version control.

### Single-file firmware package

Download `sticky-2048-v2.0.0.bin` from the GitHub release.
The merged image includes the bootloader, partition table, and application.

Flash this merged file at address **0x0** on an ESP32-S3 Sticky:

```bash
python -m esptool --chip esp32s3 -p PORT write_flash --flash_mode dio --flash_freq 80m --flash_size 32MB 0x0 sticky-2048-v2.0.0.bin
```

The package uses a 32 MB flash configuration; the file itself contains only
the required images and padding. It does not contain personal NVS game saves.

## Controls

- Swipe up, down, left, or right to move tiles.
- Tap `NEW GAME` to restart the game.
- Short-press the AI button to start a new game.
- Long-press the AI button to save the current game and enter Deep Sleep.
- Release the button after the sleep page appears, then press it again to wake
  and restore the saved game. This works with USB connected or on battery power.
  Wake restarts the application; startup releases the retained GPIO holds before
  initializing the display, touch panel, and SD power rail.

## Project Structure

```text
main/
├── game/          # 2048 game logic and NVS save state
├── hardware/      # Display, touch, battery, button, and buzzer adapters
├── ui/            # E-Ink framebuffer rendering and bitmap font
├── main.cpp       # Application entry and refresh management
└── pin_config.h

components/
├── bq27220/       # Battery fuel gauge driver
├── button/        # Button driver
├── debug_logging/ # Logging dependency used by hardware drivers
├── gt911/         # Touch controller driver
└── seeed_epaper/  # E-Ink display driver
```

## Hardware Mapping

| Function | GPIO |
|---|---:|
| AI / Power Button | 4 |
| External Power Detect | 9 |
| E-Paper SPI | 13–18 |
| E-Paper MISO | 12 |
| MicroSD CS / Enable | 8, 10 |
| GT911 I2C | 2, 3 |
| GT911 Enable / Interrupt / Reset | 42, 21, 41 |
| BQ27220 I2C | 0, 1 |
| Charger Enable | 39 |
| Buzzer PWM | 48 |
| Power Hold / Lock | 45, 46 |

## Display Compatibility

The bundled `seeed_epaper` component follows the official
Seeed-reTerminal-Sticky-Firmware implementation. It reads the controller ID
with command `0x70`: `0x07` selects SSD2677, and other values select SSD1677.
The driver applies the matching BUSY polarity, reset timing, and refresh sequence.
Startup logs report `Detected e-paper controller: SSD1677` or `SSD2677`.
Before probing, display power is enabled with RESET, DC, and CS idle high,
followed by a 100 ms power settling delay, matching the reference Demo startup.
The `panel probe` log also reports the raw controller ID for troubleshooting.

MicroSD shares SPI2 with the display. Since this game does not use SD storage,
startup holds MicroSD CS (GPIO8) high and enables its power rail (GPIO10 high)
before display initialization, matching Dashboard `board_init()` and the official
firmware's shared-SPI startup. The card is not mounted or used by the game.
Unused SPI data pins are explicitly disabled to
avoid taking GPIO0 away from the sensor I2C bus. Game saves continue to use NVS.

To check on hardware, boot each screen revision with and without a MicroSD card
inserted, confirm the detected controller in the serial log, and check the initial
full refresh, swipe-triggered partial refresh, and periodic full refresh cleanup.
Also confirm touch input and battery readings still work.

## Scope

This project focuses on the core 2048 gameplay experience and E-Ink
interaction on reTerminal Sticky.

To keep the firmware lightweight, this demo does not include Wi-Fi, BLE, OTA,
MQTT, cloud services, LVGL, or unrelated application features.

## License

MIT License
