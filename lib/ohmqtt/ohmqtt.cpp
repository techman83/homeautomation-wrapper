/**
 * Default MQTT/Network library for techman83's home automation
 */

#include <ohmqtt.h>
#include "Arduino.h"

ohmqtt::ohmqtt() {
  IPAddress broker(10,4,2,21);    // Address of the MQTT broker
  this->broker      = broker;
  this->callback    = nullptr;
  this->resubscribe = nullptr;
  this->wifi_ssid   = nullptr;
  this->wifi_pass   = nullptr;
  this->client      = nullptr;
}

ohmqtt& ohmqtt::setClient(PubSubClient* client) {
  if (client != nullptr) {
    this->client = client;
  }
  return *this;
}

ohmqtt& ohmqtt::setCallback(MQTT_CALLBACK_SIGNATURE) {
  if (callback != nullptr) {
    this->callback = callback;
  }
  return *this;
}

ohmqtt& ohmqtt::setResubscribe(MQTT_RESUBSCRIBE_SIGNATURE) {
  if (resubscribe != nullptr) {
    this->resubscribe = resubscribe;
  }
  return *this;
}

ohmqtt& ohmqtt::setClientID(const char* client_id) {
  if (client_id != nullptr) {
    this->client_id = strdup(client_id);
  }
  return *this;
}

ohmqtt& ohmqtt::setWiFi(const char* wifi_ssid, const char* wifi_pass) {
  if (wifi_ssid != nullptr) {
    this->wifi_ssid = strdup(wifi_ssid);
  }
  if (wifi_pass != nullptr) {
    this->wifi_pass = strdup(wifi_pass);
  }
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
    switch (error) {
        case OTA_AUTH_ERROR:
             Serial.println("Auth Failed");
             break;
        case OTA_BEGIN_ERROR:
            Serial.println("Begin Failed");
            break;
        case OTA_CONNECT_ERROR:
            Serial.println("Connect Failed");
            break;
        case OTA_RECEIVE_ERROR:
            Serial.println("Receive Failed");
            break;
        case OTA_END_ERROR:
            Serial.println("End Failed");
            break;
        default:
            Serial.println("Unknown Error");
            break;
    }
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
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(wifi_ssid);
    Serial.println("...");
    WiFi.begin(wifi_ssid, wifi_pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      return;
    }
    Serial.println("WiFi connected");
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (!client->connected()) {
      reconnect();
    }
  }

  if(client->connected()) {
    client->loop();
  }
}
