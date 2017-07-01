/*
 * Template for Techman83's home automation
 */

#include <ohmqtt.h>
#define BUFFER_SIZE 100

ohmqtt net;

void callback(char* topic, byte* payload, unsigned int length) {
  // Perform callback actions
}

void resubscribe() {
  // Perform resubscribe actions
}

void setup() {
	Serial.begin(115200);
  Serial.println("Main Setup");
  Serial.println(CLIENT_ID);
  net.setClientID(CLIENT_ID);
  net.setWiFi(WIFI_SSID, WIFI_PASS);
  net.setCallback(&callback);
  net.setResubscribe(&resubscribe);
  net.netSetup();
}

void loop() {
  net.netLoop();
  delay(2000);
}
