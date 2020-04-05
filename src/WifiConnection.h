#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <nvs.h>
#include <nvs_flash.h>

class WifiConnection
{

private:
  // Define your default settings here
  const char *ssid = "NicE_Engineering_UPC"; //Replace with WIFI name.
  const char *udpAddress = "192.168.4.255";
  const int port = 9876;
  const char *password = "1001100110";
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