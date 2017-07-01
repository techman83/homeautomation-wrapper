/**
 * Default MQTT/Network library for techman83's home automation
 */

#include <ohmqtt.h>
#include "Arduino.h"

ohmqtt::ohmqtt() {
  IPAddress broker(10,4,2,21);    // Address of the MQTT broker
  WiFiClient wificlient;
  this->client = new PubSubClient(wificlient);
  this->broker = broker;
  setCallback(NULL);
  setResubscribe(NULL);
  setClientID(NULL);
}

ohmqtt& ohmqtt::setCallback(MQTT_CALLBACK_SIGNATURE) {
  this->callback = callback;
  return *this;
}

ohmqtt& ohmqtt::setResubscribe(MQTT_RESUBSCRIBE_SIGNATURE) {
  this->resubscribe = resubscribe;
  return *this;
}

ohmqtt& ohmqtt::setClientID(const char* client_id) {
  this->client_id = client_id;
  return *this;
}

ohmqtt& ohmqtt::setWiFi(const char* wifi_ssid, const char* wifi_pass) {
  this->wifi_ssid = wifi_ssid;
  this->wifi_pass = wifi_pass;
  return *this;
}

void ohmqtt::netSetup() {
  WiFi.mode(WIFI_STA);
  WiFi.hostname(client_id);
  WiFi.begin(wifi_ssid, wifi_pass);
  Serial.println("WiFi begun");
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(client_id);

  client->setServer(broker, 1883);
  client->setCallback(callback);
  
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if      (error == OTA_AUTH_ERROR   ) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR  ) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR    ) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void ohmqtt::reconnect() {
  // Loop until we're reconnected
  while (!client->connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client->connect(client_id)) {
      Serial.println("connected");

      // Once connected, publish an announcement...
      String conMessage = String(client_id) + " connected";
      client->publish("/test", conMessage.c_str() );

      // ... and resubscribe
      resubscribe();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client->state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void ohmqtt::netLoop(){
  ArduinoOTA.handle();
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Connecting to ");
    Serial.print(wifi_ssid);
    Serial.println("...");
    WiFi.begin(wifi_ssid, wifi_pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
      return;
    Serial.println("WiFi connected");
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (!client->connected()) {
      reconnect();
    }
  }

  if(client->connected())
    client->loop();      
}

