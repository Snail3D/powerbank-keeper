/*
  PowerBank Keeper - ESP32-C3 SuperMini
  Keeps any USB power bank alive by pulsing WiFi TX current.
  Zero extra hardware needed. Just plug the SuperMini into the power bank.
  
  Strategy:
  - Deep sleep between pulses (~10-15µA draw)
  - Wake every 20 seconds via timer
  - Fire WiFi TX for ~500ms (spikes to 200-400mA)
  - Kill WiFi, go back to deep sleep
  
  This resets the power bank's auto-off timer.
  
  Built by Biggy Snails for Eric.
*/

#include <WiFi.h>

// Pulse interval in microseconds (20 seconds = 20,000,000 µs)
#define SLEEP_INTERVAL_US 20ULL * 1000000ULL

// WiFi TX burst duration in milliseconds
#define WIFI_BURST_MS 500

// LED pin for visual feedback (GPIO2 on most SuperMini boards, GPIO8 on some)
// Built-in LED on ESP32-C3 SuperMini is typically active-LOW
#define LED_PIN 8

void setup() {
  // Quick LED blink to show we're alive
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);   // LED ON (active low on most C3 boards)
  delay(50);
  digitalWrite(LED_PIN, HIGH);  // LED OFF
  
  // Fire up WiFi and blast a TX to spike current draw
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();  // Make sure we're not trying to connect to anything
  
  // Perform a WiFi scan - this forces TX activity (probe requests)
  // which spikes current to 200-400mA
  int n = WiFi.scanNetworks(false, false, false, WIFI_BURST_MS / 100);
  
  // Turn WiFi off immediately after the burst
  WiFi.mode(WIFI_OFF);
  WiFi.scanDelete();  // Free scan results memory
  
  // Quick double-blink to confirm pulse completed
  digitalWrite(LED_PIN, LOW);
  delay(30);
  digitalWrite(LED_PIN, HIGH);
  delay(30);
  digitalWrite(LED_PIN, LOW);
  delay(30);
  digitalWrite(LED_PIN, HIGH);
  
  // Go to deep sleep until next pulse
  esp_sleep_enable_timer_wakeup(SLEEP_INTERVAL_US);
  esp_deep_sleep_start();
}

void loop() {
  // We never get here - deep sleep resets on wake
}
