/*
 * Template for Techman83's home automation
 */

#include <ohmqtt.h>
#define BUFFER_SIZE 100

ohmqtt net;
WiFiClient wificlient;
PubSubClient client(wificlient);

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
  net.setClientID(CLIENT_ID)
    .setClient(&client)
    .setWiFi(WIFI_SSID, WIFI_PASS)
    .setCallback(&callback)
    .setResubscribe(&resubscribe)
    .netSetup();
}

void loop() {
  net.netLoop();
  delay(2000);
}
