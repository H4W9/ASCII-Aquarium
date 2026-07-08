# ASCII Aquarium — Marauder Pancake port

A port of POWER-PILL's ASCII Aquarium (v2.39) to the **Marauder Pancake**
hardware: ESP32-C5, ST7796 320×480 panel, FT6336 capacitive touch. The aquarium
runs **full-screen in 480×320 landscape** (hold the device sideways).

## What changed vs. the stock CYD sketch

The sketch keeps all the original features and adds a new board profile,
`AQUARIUM_BOARD_PANCAKE` (defined at the very top of the `.ino`). Compared with
the CYD/ST7796U resistive-touch builds:

| Area | Pancake behaviour |
|------|-------------------|
| Display | ST7796, TFT_eSPI rotation 1 → 480×320, aquarium fills the whole panel |
| Render surface | Forced into internal RAM and rendered in horizontal strips (a full 480×320 canvas would land in slow PSRAM) |
| Touch | FT6336U over I2C (`ft6336_touch.h`), SDA=9 / SCL=10 / RST=8, addr 0x38 |
| SD card | Shares the FSPI bus with the TFT (SD_CS=7, shared MISO/MOSI/SCLK) |
| Backlight | GPIO 26 (PWM) |
| RGB ambient LEDs | none — those controls are inert on Pancake |
| BOOT-button screenshot | disabled (no button wired); use the on-screen **Capture** panel instead |
| UI panels | re-centred for the 480-wide screen via `UI_X_SHIFT` |

## Required libraries

> **Critical:** stock TFT_eSPI does **not** support the ESP32-C5. You must build
> against the **ESP32-C5 fork of TFT_eSPI** that ships with your Marauder Bible
> firmware:
> `.../MarauderBible/bible_firmware/libraries/TFT_eSPI-ESP32-C5`

Point the Arduino IDE at that library (or copy it into your Arduino
`libraries/` folder). Then configure it for the Pancake panel exactly as the
Bible firmware does:

In `TFT_eSPI-ESP32-C5/User_Setup_Select.h`, make sure this line is active:

```cpp
#include <User_Setup_marauder_pancake.h>
//#include <User_Setup_marauder_v8.h>
//#include <User_Setup_og_marauder.h>
```

A copy of that setup (ST7796, C5 pins, `TFT_INVERSION_ON`) is included here as
`User_Setup_marauder_pancake.h` for reference — the pin numbers in the sketch
match it. Other libraries used: `SD`, `Preferences`, `WiFi` (all bundled with the
ESP32 core). `XPT2046_Touchscreen` is **not** needed for the Pancake build.

## Arduino IDE settings (same core as the Bible firmware)

| Setting | Value |
|---------|-------|
| Board | ESP32C5 Dev Module |
| Flash Size | 8 MB |
| PSRAM | Enabled |
| Partition Scheme | Default (or any scheme with ≥1.5 MB app) |

This is a standalone app (it does **not** use the Bible's dual-boot
`partitions.csv`); a normal single-app partition scheme is fine.

## Build & flash

1. Open `ASCII_Aquarium_Pancake/ASCII_Aquarium_Pancake.ino` in the Arduino IDE.
2. Confirm `#define AQUARIUM_BOARD_PANCAKE` is present at the top (it is by default).
3. Make sure TFT_eSPI-ESP32-C5 is selected for the Pancake ST7796 setup (above).
4. Select the ESP32C5 Dev Module board and your serial port.
5. Upload.

On boot you should see a short diagnostic splash (render size, heap/PSRAM,
`Touch: OK`) then the aquarium.

## Building in GitHub Actions (no local toolchain needed)

A workflow at `.github/workflows/build-pancake.yml` compiles this sketch in the
cloud against the H4W9 ESP32-C5 fork of TFT_eSPI
(`https://github.com/H4W9/TFT_eSPI` branch `ESP32-C5`) and publishes the compiled
`.bin`/`.elf` as a downloadable artifact.

What it does:
1. Installs Arduino CLI + the ESP32 core (`ESP32_CORE_VERSION`, default `3.3.0`).
2. `git clone --branch ESP32-C5` of the TFT_eSPI fork into the sketchbook.
3. Forces the Pancake ST7796/C5 setup by copying
   `User_Setup_marauder_pancake.h` into the library and pointing
   `User_Setup_Select.h` at it — so the build is deterministic regardless of the
   fork's default setup selection.
4. Compiles for `esp32:esp32:esp32c5` (`PartitionScheme=huge_app,PSRAM=enabled`).
5. Uploads the binaries as artifact `ascii-aquarium-pancake-<sha>`.

To run it: push this folder + the workflow to `H4W9/ASCII-Aquarium`, then either
push a change under `ASCII_Aquarium_Pancake/` or trigger it manually from the
repo's **Actions → Build ASCII Aquarium (Pancake) → Run workflow**. Download the
firmware from the run's **Artifacts** section.

Knobs live in the workflow's `env:` block (core version, TFT_eSPI repo/branch).
If the ESP32 core rejects a `--board-options` key, the **Show board options**
step in the log lists the valid keys for your core version.

## Touch orientation tuning

The FT6336 reports coordinates in the panel's native portrait frame; the sketch
rotates them into the 480×320 landscape scene in `readCapTouchPoint()`. If taps
land **mirrored** on your unit, flip the flags near that function:

```cpp
static const bool CAP_TOUCH_INVERT_X = false;  // long (480) axis
static const bool CAP_TOUCH_INVERT_Y = false;  // short (320) axis
```

- Taps mirror **left↔right** → set `CAP_TOUCH_INVERT_X = true`.
- Taps mirror **up↕down** → set `CAP_TOUCH_INVERT_Y = true`.

The **Flip Display** option (Tank settings) also swaps the whole scene between
TFT rotation 1 and 3; touch follows it automatically.

## SD-card capture

Screenshots / frame sequences (BMP) save to the SD card over the shared FSPI
bus. Open the HUD (tap the top-left corner) → **Capture**. Capture is slow by
design (the sim is throttled so no frames are skipped).

## Known trade-offs

- **Frame rate:** 480×320 is ~2× the CYD's pixel count pushed at 27 MHz, so
  expect a calmer ~10–15 fps. Fine for an ambient aquarium. If your panel is
  stable at a higher `SPI_FREQUENCY`, raise it in `User_Setup_marauder_pancake.h`.
- If the render surface can't fit even the smallest strip in internal RAM
  (heavy WiFi use), the splash shows `Sprite alloc failed`; disable Wi-Fi or
  lower `BACKGROUND_GRADIENT_H`.
