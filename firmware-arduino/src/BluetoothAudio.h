#ifndef BLUETOOTH_AUDIO_H
#define BLUETOOTH_AUDIO_H

#include <Arduino.h>
#include "Config.h"

// ESP32S3 doesn't support A2DP Bluetooth audio
#ifndef XIAO_ESP32S3
#include "BluetoothA2DPSink.h"
#include "BluetoothA2DPSource.h"
#endif

#include "AudioTools.h"

// Target Bluetooth device name
extern const char* BT_DEVICE_NAME;

#ifndef XIAO_ESP32S3
// Bluetooth A2DP objects
extern BluetoothA2DPSink a2dp_sink;
extern BluetoothA2DPSource a2dp_source;
#endif

// Flags to track connection status
extern bool bt_connected;
extern bool bt_sink_connected;
extern bool bt_source_connected;

// Initialize Bluetooth audio
void setupBluetoothAudio();

// Connect to a specific Bluetooth device by name 
bool connectToBluetoothDevice(const char* device_name);

// Disconnect from Bluetooth device
void disconnectBluetoothDevice();

// Send audio data to Bluetooth speaker
void sendAudioToBluetooth(const uint8_t* data, size_t len);

#ifndef XIAO_ESP32S3
// Callback function for connection events
void bt_connection_state_changed(esp_a2d_connection_state_t state, void *);
#endif

// Send audio from the audio buffer to the connected Bluetooth device
void bluetoothAudioTask(void* parameter);

#endif // BLUETOOTH_AUDIO_H 