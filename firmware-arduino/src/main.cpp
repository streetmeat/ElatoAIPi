#include "OTA.h"
#include <Arduino.h>
#include <driver/rtc_io.h>
#include "LEDHandler.h"
#include "Config.h"
// Remove SPIFFS include for ESP32S3
#ifdef XIAO_ESP32S3
// SPIFFS is not supported on ESP32S3
#else
#include "SPIFFS.h"
#endif
#include "WifiManager.h"
#include <driver/touch_sensor.h>
#include "Button.h"
#include "FactoryReset.h"

// #define WEBSOCKETS_DEBUG_LEVEL WEBSOCKETS_LEVEL_ALL

#define TOUCH_THRESHOLD 28000
#define LONG_PRESS_MS 1000
#define REQUIRED_RELEASE_CHECKS 100     // how many consecutive times we need "below threshold" to confirm release
#define TOUCH_DEBOUNCE_DELAY 1000 // milliseconds

AsyncWebServer webServer(80);
WIFIMANAGER WifiManager;
esp_err_t getErr = ESP_OK;

// Initialize file system only for non-ESP32S3 boards
bool initializeFileSystem() {
#ifndef XIAO_ESP32S3
    // ESP32S3 has issues with the LittleFS/SPIFFS implementation
    if (!SPIFFS.begin()) {
        Serial.println("SPIFFS initialization failed!");
        return false;
    }
    Serial.println("SPIFFS initialized successfully!");
    return true;
#else
    Serial.println("SPIFFS initialization skipped for ESP32S3");
    return true;
#endif
}

void enterSleep()
{
    Serial.println("Going to sleep...");
    
    // First, change device state to prevent any new data processing
    deviceState = IDLE;

    // Stop audio tasks first
    i2s_stop(I2S_PORT_IN);
    i2s_stop(I2S_PORT_OUT);

    // Clear any remaining audio in buffer
    audioBuffer.reset();
    
    // Properly disconnect WebSocket and wait for it to complete
    if (webSocket.isConnected()) {
        webSocket.disconnect();
        // Give some time for the disconnect to process
        delay(100);
    }
    
    // Stop all tasks that might be using I2S or other peripherals
    i2s_driver_uninstall(I2S_PORT_IN);
    i2s_driver_uninstall(I2S_PORT_OUT);
    
    // Flush any remaining serial output
    Serial.flush();

    #ifdef TOUCH_MODE
        touch_pad_intr_disable(TOUCH_PAD_INTR_MASK_ALL);
        while (touchRead(TOUCH_PAD_NUM2) > TOUCH_THRESHOLD) {
        delay(50);
        }
        delay(500);
        touchSleepWakeUpEnable(TOUCH_PAD_NUM2, TOUCH_THRESHOLD);
    #endif

    esp_deep_sleep_start();
}

void printOutESP32Error(esp_err_t err)
{
    switch (err)
    {
    case ESP_OK:
        Serial.println("ESP_OK no errors");
        break;
    case ESP_ERR_INVALID_ARG:
        Serial.println("ESP_ERR_INVALID_ARG if the selected GPIO is not an RTC GPIO, or the mode is invalid");
        break;
    case ESP_ERR_INVALID_STATE:
        Serial.println("ESP_ERR_INVALID_STATE if wakeup triggers conflict or wireless not stopped");
        break;
    default:
        Serial.printf("Unknown error code: %d\n", err);
        break;
    }
}

static void onButtonLongPressUpEventCb(void *button_handle, void *usr_data)
{
    Serial.println("Button long press end");
    delay(10);
    enterSleep();
}

static void onButtonDoubleClickCb(void *button_handle, void *usr_data)
{
    Serial.println("Button double click");
    delay(10);
    enterSleep();
}

void getAuthTokenFromNVS()
{
    preferences.begin("auth", false);
    authTokenGlobal = preferences.getString("auth_token", "");
    preferences.end();
}

void setupWiFi()
{
    WifiManager.startBackgroundTask("ELATO-DEVICE");        // Run the background task to take care of our Wifi
    WifiManager.fallbackToSoftAp(true);       // Run a SoftAP if no known AP can be reached
    WifiManager.attachWebServer(&webServer);  // Attach our API to the Webserver 
    WifiManager.attachUI();                   // Attach the UI to the Webserver
  
    // Run the Webserver and add your webpages to it
    webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->redirect("/wifi");
    });
    webServer.onNotFound([&](AsyncWebServerRequest *request) {
      request->send(404, "text/plain", "Not found");
    });
    webServer.begin();
}


void touchTask(void* parameter) {
  touch_pad_init();
  touch_pad_config(TOUCH_PAD_NUM2);

  bool touched = false;
  unsigned long pressStartTime = 0;
  unsigned long lastTouchTime = 0;
  const unsigned long LONG_PRESS_DURATION = 500; // 500ms for long press

  while (1) {
    // Read the touch sensor
    uint32_t touchValue = touchRead(TOUCH_PAD_NUM2);
    bool isTouched = (touchValue > TOUCH_THRESHOLD);
    unsigned long currentTime = millis();

    // Initial touch detection
    if (isTouched && !touched && (currentTime - lastTouchTime > TOUCH_DEBOUNCE_DELAY)) {
      touched = true;
      pressStartTime = currentTime;  // Start timing the press
      lastTouchTime = currentTime;
    }

    // Check for long press while touched
    if (touched && isTouched) {
      if (currentTime - pressStartTime >= LONG_PRESS_DURATION) {
        enterSleep();  // Only enter sleep after 500ms of continuous touch
      }
    }

    // Release detection
    if (!isTouched && touched) {
      touched = false;
      pressStartTime = 0;  // Reset the press timer
    }

    vTaskDelay(20);  // Reduced from 50ms to 20ms for better responsiveness
  }
  vTaskDelete(NULL);
}


void setupDeviceMetadata() {
    // factoryResetDevice();
    deviceState = IDLE;

    getAuthTokenFromNVS();
    getOTAStatusFromNVS();

    if (otaState == OTA_IN_PROGRESS || otaState == OTA_COMPLETE) {
        deviceState = OTA;
    }
    if (factory_reset_status) {
        deviceState = FACTORY_RESET;
    }
}

void setup()
{
    Serial.begin(115200);
    delay(3000); // Wait longer for serial monitor connection
    Serial.println("--- Starting Setup ---");

    // SETUP
    Serial.println("Setting up device metadata...");
    setupDeviceMetadata();
    Serial.println("Creating wsMutex...");
    wsMutex = xSemaphoreCreateMutex();
    Serial.println("wsMutex created.");

    // Initialize file system (will skip on ESP32S3)
    Serial.println("Initializing filesystem...");
    initializeFileSystem();
    Serial.println("Filesystem initialized.");

    // INTERRUPT
    #ifdef TOUCH_MODE
        Serial.println("Creating Touch Task...");
        xTaskCreate(touchTask, "Touch Task", 4096, NULL, configMAX_PRIORITIES-2, NULL);
        Serial.println("Touch Task created.");
    #else
        Serial.println("Setting up Button Interrupt...");
        getErr = esp_sleep_enable_ext0_wakeup(BUTTON_PIN, LOW);
        printOutESP32Error(getErr);
        Button *btn = new Button(BUTTON_PIN, false);
        btn->attachLongPressUpEventCb(&onButtonLongPressUpEventCb, NULL);
        btn->attachDoubleClickEventCb(&onButtonDoubleClickCb, NULL);
        btn->detachSingleClickEvent();
        Serial.println("Button Interrupt setup done.");
    #endif

    // Pin audio tasks to Core 1 (application core)
    Serial.println("Creating LED Task...");
    xTaskCreatePinnedToCore(
        ledTask,           // Function
        "LED Task",        // Name
        4096,              // Stack size
        NULL,              // Parameters
        5,                 // Priority
        NULL,              // Handle
        1                  // Core 1 (application core)
    );
    Serial.println("LED Task created.");

    Serial.println("Creating Speaker Task...");
    xTaskCreatePinnedToCore(
        audioStreamTask,   // Function
        "Speaker Task",    // Name
        4096,              // Stack size
        NULL,              // Parameters
        3,                 // Priority
        NULL,              // Handle
        1                  // Core 1 (application core)
    );
    Serial.println("Speaker Task created.");

    Serial.println("Creating Microphone Task...");
    xTaskCreatePinnedToCore(
        micTask,           // Function
        "Microphone Task", // Name
        4096,              // Stack size
        NULL,              // Parameters
        4,                 // Priority
        NULL,              // Handle
        1                  // Core 1 (application core)
    );
    Serial.println("Microphone Task created.");


    // Pin network task to Core 0 (protocol core)
    Serial.println("Creating Network Task...");
    xTaskCreatePinnedToCore(
        networkTask,       // Function
        "Websocket Task",  // Name
        8192,              // Stack size
        NULL,              // Parameters
        configMAX_PRIORITIES-1, // Highest priority
        &networkTaskHandle,// Handle
        0                  // Core 0 (protocol core)
    );
    Serial.println("Network Task created.");


    // Init Bluetooth and create Bluetooth task
    #ifdef XIAO_ESP32S3
        // ESP32S3 does not support A2DP Bluetooth audio
        // Using serial audio output for testing instead
        Serial.println("Setting up Serial Audio for ESP32S3...");

        // Create a task for serial audio output testing
        Serial.println("Creating Serial Audio Task...");
        xTaskCreatePinnedToCore(
            serialAudioOutputTask, // Function
            "Serial Audio",        // Name
            4096,                  // Stack size
            NULL,                  // Parameters
            2,                     // Priority
            NULL,                  // Handle
            1                      // Core 1 (application core)
        );
        Serial.println("Serial Audio Task created.");

        /*
        setupBluetoothAudio();

        xTaskCreatePinnedToCore(
            bluetoothAudioTask, // Function
            "Bluetooth Audio",  // Name
            4096,               // Stack size
            NULL,               // Parameters
            2,                  // Priority
            NULL,               // Handle
            1                   // Core 1 (application core)
        );
        */
    #endif

    // WIFI
    Serial.println("Setting up WiFi...");
    setupWiFi();
    Serial.println("WiFi setup called.");

    Serial.println("--- Setup Complete ---");
}

void loop(){
    if (otaState == OTA_IN_PROGRESS)
    {
        loopOTA();
    }
}