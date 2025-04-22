#include "BluetoothAudio.h"
#include "Audio.h"

// Target Bluetooth device name - Bose SoundLink Flex
const char* BT_DEVICE_NAME = "Bose Flex SoundLink";

// Flags to track connection status
bool bt_connected = false;
bool bt_sink_connected = false;
bool bt_source_connected = false;

#ifndef XIAO_ESP32S3
// Bluetooth A2DP objects
BluetoothA2DPSink a2dp_sink;
BluetoothA2DPSource a2dp_source;

// Connection state callback
void bt_connection_state_changed(esp_a2d_connection_state_t state, void *) {
    if (state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
        Serial.println("Bluetooth connected!");
        bt_connected = true;
        bt_source_connected = true;
    } else if (state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
        Serial.println("Bluetooth disconnected!");
        bt_connected = false;
        bt_source_connected = false;
    }
}

// Data callback for when we receive audio
void bt_data_callback(const uint8_t *data, uint32_t len) {
    // Process incoming audio data if needed
    // For now, we're just focusing on output to the speaker
}
#endif

// Initialize Bluetooth audio
void setupBluetoothAudio() {
    Serial.println("Setting up Bluetooth audio...");
    
#ifndef XIAO_ESP32S3
    // Set up A2DP source (for sending audio to speaker)
    a2dp_source.set_auto_reconnect(true);
    a2dp_source.set_on_connection_state_changed(bt_connection_state_changed);
    a2dp_source.start("ESP32-TTS-Source");
    
    // Try to connect to the Bose speaker automatically
    connectToBluetoothDevice(BT_DEVICE_NAME);
    
    // Set up A2DP sink if we need to receive audio in the future
    // a2dp_sink.set_stream_reader(bt_data_callback);
    // a2dp_sink.set_on_connection_state_changed(bt_connection_state_changed);
    // a2dp_sink.start("ESP32-TTS-Sink");
#else
    Serial.println("ESP32S3 does not support A2DP Bluetooth audio");
#endif
}

// Connect to a specific Bluetooth device by name
bool connectToBluetoothDevice(const char* device_name) {
    Serial.printf("Attempting to connect to Bluetooth device: %s\n", device_name);
    
#ifndef XIAO_ESP32S3
    // Start scanning for the device
    if (a2dp_source.get_connection_state() == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
        return a2dp_source.connect_by_name(device_name);
    }
#else
    Serial.println("ESP32S3 does not support A2DP Bluetooth audio");
#endif
    
    return false;
}

// Disconnect from Bluetooth device
void disconnectBluetoothDevice() {
#ifndef XIAO_ESP32S3
    if (bt_source_connected) {
        a2dp_source.disconnect();
    }
    
    if (bt_sink_connected) {
        a2dp_sink.disconnect();
    }
#else
    Serial.println("ESP32S3 does not support A2DP Bluetooth audio");
#endif
}

// Send audio data to Bluetooth speaker
void sendAudioToBluetooth(const uint8_t* data, size_t len) {
#ifndef XIAO_ESP32S3
    if (bt_connected && bt_source_connected) {
        a2dp_source.write_data(data, len);
    }
#else
    Serial.println("ESP32S3 does not support A2DP Bluetooth audio");
#endif
}

// Task for continuously sending audio from buffer to Bluetooth
void bluetoothAudioTask(void* parameter) {
    uint8_t buffer[512];
    size_t bytes_read;
    
    while (true) {
#ifndef XIAO_ESP32S3
        if (bt_connected && deviceState == SPEAKING) {
            // Read from our audio buffer if available
            bytes_read = audioBuffer.readArray(buffer, sizeof(buffer));
            
            if (bytes_read > 0) {
                // Send to Bluetooth speaker
                sendAudioToBluetooth(buffer, bytes_read);
            }
        }
#else
        // For ESP32S3, we don't do anything in this task
        // The serialAudioOutputTask handles audio output for testing
#endif
        
        // Small delay to prevent CPU hogging
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
    
    vTaskDelete(NULL);
} 