# ESP32 WebSocket Audio Client

This firmware turns your ESP32 device into a WebSocket audio client for Elato, enabling real-time conversations with AI characters.

## Hardware Setup

### Components Needed
- ESP32-S3 board
- I2S MEMS microphone (INMP441 recommended)
- I2S speaker with amplifier (MAX98357A recommended)
- Microspeaker
- Button/Touch sensor and LED (optional but recommended)

### Pin Connections

| **Component** | **Standard ESP32** |
|---------------|-------------------|
| **Microphone** |                   |
| SD (Data)     | GPIO 14           |
| WS (Word Select)        | GPIO 4            |
| SCK (Clock)            | GPIO 1            |
| **Speaker**   |                  |                   |
| WS                    | GPIO 5            |
| BCK             | GPIO 6            |
| DATA             | GPIO 7            |
| SD (shutdown)         | GPIO 10           |
| **Control**             |                   |
| Button               | GPIO 2            |
| LED (Blue)              | GPIO 13           |
| LED (Red)           | GPIO 9            |
| LED (Green)           | GPIO 8            |

## Software Setup

### Using PlatformIO

1. Install Visual Studio Code and the PlatformIO extension
2. Clone this repository
3. Open the project folder in PlatformIO
4. Edit `src/Config.cpp` with your server details:
   - If using locally: Set your computer's IP address in `ws_server` and `backend_server`
   - If using production: Ensure proper certificates are set
5. Build and upload to your ESP32


### To use locally:
1. **Find your local IP address**:
   - View your Wifi IP when you click on Wifi Settings > Your Wifi Network > Details, OR 
   - On macOS/Linux: Open Terminal and run `ifconfig`
   - On Windows: Open Command Prompt and run `ipconfig`
   - Look for your active network interface (WiFi: `en0` on Mac, `wlan0` on Linux, `Wireless LAN adapter Wi-Fi` on Windows)
   - Note the IP address (e.g., `192.168.1.100`)

2. **Update firmware configuration**:
   - In the firmware project, set `DEV_MODE` in Config.cpp
   - Update the WebSocket server IP to your local IP address

## First-Time Setup

1. Power on your ESP32
2. Connect to the "Elato device" WiFi network from your phone/computer
3. A configuration portal will open (or navigate to 192.168.4.1)
4. Enter your home WiFi credentials
5. The device will restart and connect to your WiFi

## Usage

1. Power on the device
2. The LED indicates status:
   - Green 🟢: Setup mode and websocket/wifi is not connected
   - Blue 🔵: Device is speaking
   - Yellow 🟡: Device is listening to user
   - Red 🔴: Processing user request 

## Troubleshooting

- If connection fails, check your WiFi signal and server details
- Monitor serial output at 115200 baud for detailed logs

## Advanced Configuration

Edit `Config.cpp` to customize:
- Server addresses and ports
- Audio sample rate (default 24kHz)
- Pin assignments for different boards

For development, uncomment `#define DEV_MODE` in Config.h to use local servers without SSL.

To use the button, uncomment `#define TOUCH_MODE` in Config.h