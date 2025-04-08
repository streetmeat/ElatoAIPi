#include "Config.h"
#include <nvs_flash.h>

// ! define preferences
Preferences preferences;
OtaStatus otaState = OTA_IDLE;
bool factory_reset_status = false;

// websocket_setup("192.168.1.166", 8000, "/");
// websocket_setup("talkedge.deno.dev",443, "/");
// websocket_setup("xygbupeczfhwamhqnucy.supabase.co", 443, "/functions/v1/relay");
// websocket_setup("https://emkmtesvjrqhvx2mo2mxslvmmy0zsuhq.lambda-url.us-east-1.on.aws/", 8000, "/");
// Runtime WebSocket server details

#ifdef DEV_MODE
const char *ws_server = "10.2.1.25";
const uint16_t ws_port = 8000;
const char *ws_path = "/";
// Backend server details 
const char *backend_server = "10.2.1.25";
const uint16_t backend_port = 3000;

#else
// PROD
const char *ws_server = "talkedge.deno.dev";
const uint16_t ws_port = 443;
const char *ws_path = "/";
// Backend server details 
const char *backend_server = "www.elatoai.com";
const uint16_t backend_port = 3000;

#endif

String authTokenGlobal;
DeviceState deviceState = IDLE;

// I2S and Audio parameters
const uint32_t SAMPLE_RATE = 24000;

// ----------------- Pin Definitions -----------------
const i2s_port_t I2S_PORT_IN = I2S_NUM_1;
const i2s_port_t I2S_PORT_OUT = I2S_NUM_0;

#ifdef USE_NORMAL_ESP32

const int BLUE_LED_PIN = 13;
const int RED_LED_PIN = 9;
const int GREEN_LED_PIN = 8;

const int I2S_SD = 14;
const int I2S_WS = 4;
const int I2S_SCK = 1;

const int I2S_WS_OUT = 5;
const int I2S_BCK_OUT = 6;
const int I2S_DATA_OUT = 7;
const int I2S_SD_OUT = 10;

const gpio_num_t BUTTON_PIN = GPIO_NUM_2; // Only RTC IO are allowed - ESP32 Pin example

#endif

// supabase CA cert
// const char *CA_cert = R"EOF(
// -----BEGIN CERTIFICATE-----
// <YOUR HOST CERTIFICATE HERE>
// -----END CERTIFICATE-----
// )EOF";

const char *Vercel_CA_cert = R"EOF(
-----BEGIN CERTIFICATE-----
<YOUR VERCEL CERTIFICATE HERE>
-----END CERTIFICATE-----
)EOF";


// talkedge.deno.dev CA cert
const char *CA_cert = R"EOF(
-----BEGIN CERTIFICATE-----
<YOUR TALKEDGE CERTIFICATE HERE>
-----END CERTIFICATE-----
)EOF";
