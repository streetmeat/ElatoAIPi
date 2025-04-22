// ESP32S3 Simple Blink Test
// This file can be used to verify basic functionality of the ESP32S3 hardware

#include <Arduino.h>

// Define the LED pin for XIAO ESP32S3 - GPIO pin D13 is the built-in orange LED
#define LED_PIN 13

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\nXIAO ESP32S3 Simple Blink Test");
  Serial.println("------------------------------");
  
  // Initialize the LED pin as an output
  pinMode(LED_PIN, OUTPUT);
  
  // Print chip information
  Serial.printf("ESP32 Chip Model: %s\n", ESP.getChipModel());
  Serial.printf("ESP32 Chip Revision: %d\n", ESP.getChipRevision());
  Serial.printf("ESP32 SDK Version: %s\n", ESP.getSdkVersion());
  Serial.printf("ESP32 CPU Freq: %d MHz\n", ESP.getCpuFreqMHz());
  Serial.printf("ESP32 Flash Size: %d bytes\n", ESP.getFlashChipSize());
  Serial.printf("ESP32 Flash Speed: %d Hz\n", ESP.getFlashChipSpeed());
  Serial.printf("ESP32 Free Heap: %d bytes\n", ESP.getFreeHeap());
}

void loop() {
  // Blink the LED: turn it ON (LOW for XIAO ESP32S3)
  digitalWrite(LED_PIN, LOW);
  Serial.println("LED ON");
  delay(1000);
  
  // Turn LED OFF (HIGH for XIAO ESP32S3)
  digitalWrite(LED_PIN, HIGH);
  Serial.println("LED OFF");
  delay(1000);
} 