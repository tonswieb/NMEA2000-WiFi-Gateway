#ifndef _WIFICONNECTION_H_
#define _WIFICONNECTION_H_

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WiFi.h>
#include <WiFiUdp.h>

class WifiConnection
{

private:
  // Define your default settings here
  const char *udpAddress = "192.168.4.255";
  const int port = 9876;
  const char *host = "N2K-bridge";
  IPAddress ipLok;
  WiFiUDP udp;

  void ResetWiFiSettingsOnNvs();
  void setupWIFI();
  void setup_OTA();

public:
  WifiConnection();
  void sendUdpPackage(char *message);
};

#endif