# ESP32 TTS Firmware for Seeed Studio XIAO ESP32S3

This firmware is adapted for the Seeed Studio XIAO ESP32S3 board and includes Bluetooth audio support for connecting to the Bose Flex SoundLink speaker.

## Hardware Requirements

- Seeed Studio XIAO ESP32S3 board
- Bose Flex SoundLink Bluetooth speaker
- (Optional) I2S microphone
- (Optional) I2S speaker/amplifier

## Pin Assignments

The firmware uses the following pin assignments for the XIAO ESP32S3:

| Function | XIAO ESP32S3 Pin |
|----------|------------------|
| LED (Blue/Orange) | D10 |
| LED (Red) | D11 |
| LED (Green) | D12 |
| I2S Microphone Data | D2 |
| I2S Microphone WS | D3 |
| I2S Microphone SCK | D4 |
| I2S Speaker WS | D5 |
| I2S Speaker BCK | D6 |
| I2S Speaker Data | D7 |
| I2S Speaker SD | D8 |
| Touch Button | GPIO2 (Touch Pad 2) |

## Important Changes

1. **Board Configuration**: Changed from ESP32-S3-DevKitC-1 to Seeed Studio XIAO ESP32S3
2. **Flash Size**: Adjusted from 16MB to 4MB (XIAO ESP32S3 has 4MB of flash)
3. **Pin Assignments**: Updated for the XIAO ESP32S3 pinout
4. **Backend Port**: Changed from 3000 to 3002
5. **Added Bluetooth Support**: Implemented A2DP source for Bose Flex SoundLink connection

## Setup and Installation

1. Install Arduino IDE and add ESP32 board package with version 2.0.8 or higher
2. Add the ESP32 board manager URL: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
3. Install the Seeed XIAO ESP32S3 board from the boards manager
4. Open this project in PlatformIO or Arduino IDE
5. Connect your XIAO ESP32S3 via USB
6. Upload the firmware

## Using Bluetooth Audio

The firmware will automatically attempt to connect to the Bose Flex SoundLink speaker. Make sure:

1. The Bose speaker is powered on
2. It's in pairing mode (check the speaker's manual for instructions)
3. The speaker name matches "Bose Flex SoundLink" - if your device has a different name, update `BT_DEVICE_NAME` in BluetoothAudio.cpp

## Troubleshooting

- If the board is not detected, press the BOOT button while connecting
- If the upload fails, press the RESET button on the board
- For Bluetooth issues, check that the speaker is in pairing mode and within range

## Reference

For more details on the XIAO ESP32S3, refer to the [official documentation](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/). 