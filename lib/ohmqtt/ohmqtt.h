#ifndef ohmqtt_h
#define ohmqtt_h

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>

#ifdef __cplusplus
extern "C" {
#endif

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
  ESP8266WiFiClass* _WiFi;
  MQTT_CALLBACK_SIGNATURE;
  MQTT_RESUBSCRIBE_SIGNATURE;
  const char* client_id;
  const char* wifi_ssid;
  const char* wifi_pass;
public:
  ohmqtt();
  ohmqtt& setCallback(MQTT_CALLBACK_SIGNATURE);
  ohmqtt& setResubscribe(MQTT_RESUBSCRIBE_SIGNATURE);
  ohmqtt& setWiFi(const char* wifi_ssid, const char* wifi_pass);
  ohmqtt& setClientID(const char*);
  void netSetup();

  void reconnect();
  void netLoop();
};
#ifdef __cplusplus
}
#endif

#endif
