#ifndef _WIFICONNECTION_H_
#define _WIFICONNECTION_H_

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <N2KPreferences.h>

class WifiConnection
{

private:
  N2KPreferences *prefs;
  IPAddress ipLok;
  WiFiUDP udp;

  void setupWIFI();
  void setup_OTA();

public:
  WifiConnection(N2KPreferences *prefs);
  ~WifiConnection();
  void begin();
  void end();
  void sendUdpPackage(char *message);
};

#endif