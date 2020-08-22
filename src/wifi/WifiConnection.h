#ifndef _WIFICONNECTION_H_
#define _WIFICONNECTION_H_

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <prefs/N2KPreferences.h>
#include "UdpStream.h"
#include "util/Log.h"

class WifiConnection
{

private:
  N2KPreferences *prefs;
  IPAddress ipLok;
  WiFiUDP udp;
  UdpStream udpStream = UdpStream(&udp);
  Logger *logger;

  void setupWIFI();
  void setup_OTA();

public:
  WifiConnection(N2KPreferences *prefs, Logger* logger);
  ~WifiConnection();
  void begin();
  void end();
  void loop();
  void sendUdpPackage(char *message);
  Stream* getUdpPackageStream();
};

#endif