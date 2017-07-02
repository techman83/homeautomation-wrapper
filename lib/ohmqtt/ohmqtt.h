#ifndef __OHMQTT_H
#define __OHMQTT_H

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <cstdlib>

#ifdef ESP8266
    #include <functional>
    #define MQTT_CALLBACK_SIGNATURE std::function<void(char*, uint8_t*, unsigned int)> callback
    #define MQTT_RESUBSCRIBE_SIGNATURE std::function<void()> resubscribe
#else
    #define MQTT_CALLBACK_SIGNATURE void (*callback)(char*, uint8_t*, unsigned int)
    #define MQTT_RESUBSCRIBE_SIGNATURE void (*resubscribe)()
#endif

class ohmqtt {
private:
  PubSubClient* client;
  IPAddress broker;
  MQTT_CALLBACK_SIGNATURE;
  MQTT_RESUBSCRIBE_SIGNATURE;
  char* client_id;
  char* wifi_ssid;
  char* wifi_pass;
public:
  ohmqtt();
  ohmqtt& setClient(PubSubClient* client);
  ohmqtt& setCallback(MQTT_CALLBACK_SIGNATURE);
  ohmqtt& setResubscribe(MQTT_RESUBSCRIBE_SIGNATURE);
  ohmqtt& setWiFi(const char* wifi_ssid, const char* wifi_pass);
  ohmqtt& setClientID(const char*);
  void netSetup();

  void reconnect();
  void netLoop();
};

#endif
