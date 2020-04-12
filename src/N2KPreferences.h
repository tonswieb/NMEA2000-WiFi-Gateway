#ifndef _N2KPREFERENCES_H_
#define _N2KPREFERENCES_H_

#include <Preferences.h>
#include <nvs.h>
#include <nvs_flash.h>

class N2KPreferences {

private:
  const String WIFI_AP_DEFAULT_SSID = "N2K-Bridge";
  const String WIFI_AP_DEFAULT_PASSWORD = "ChangeMe";

  Preferences prefs;
  bool blEnabled;
  bool demoEnabled;

  //NMEA preferences
  bool nmeaToSerial;
  bool nmeaToWebSocket;
  bool nmeaToBluetooth;
  bool nmeaToUdp;

  //TODO: Move WIFI preferences to own class
  //WIFI General preferences
  int udpPort;

  //WIFI Station preferences
  bool staEnabled;
  String stationHostname;
  String staSSID;
  String staPassword;
  
  //WIFI AP preferences
  String apSSID;
  String apPassword;

  /**
   * Initialize fields from preferences namespace or initialize with default 
   * value if field is not (yet) stored in preference namespace.
   */
  void init();

public:
  const char *PREF_SPACE = "N2k-bridge";
  const char *PREF_BLUETOOTH_ENABLED = "blEnabled";
  const char *PREF_DEMO_ENABLED = "demoEnabled";
  const char *PREF_WIFI_STA_ENABLED = "wifiStaEnabled";
  const char *PREF_WIFI_STA_HOSTNAME = "wifiStaHostname";
  const char *PREF_WIFI_STA_SSID = "wifiStaSsid";
  const char *PREF_WIFI_STA_PASSWORD = "wifiStaPassw";
  const char *PREF_WIFI_AP_SSID = "wifiApSsid";
  const char *PREF_WIFI_AP_PASSWORD = "wifiApPassword";
  const char *PREF_WIFI_UDP_PORT = "wifiUdpPort";
  const char *PREF_NMEA_TO_SOCKET = "nmeaToSocket";
  const char *PREF_NMEA_TO_SERIAL = "nmeaToSerial";
  const char *PREF_NMEA_TO_BL = "nmeaToBl";
  const char *PREF_NMEA_TO_UDP = "nmeaToUDP";

  N2KPreferences();
  ~N2KPreferences();

  void begin();
  void end();
  /**
   * Reset all preferences set through this class and reboot the ESP.
   */
  void reset();
  /**
   * Erase the complete NVS Partition. and reboot the ESP. Which removes the preferences set 
   * by this class as well as the preferences set in different namespaces from 
   * other components.
   */
  void factoryReset();
  void setBlEnabled(bool value);
  bool isBlEnabled();
  void setDemoEnabled(bool value);
  bool isDemoEnabled();

  //WIFI General
  void setUdpBroadcastPort(int port);
  int getUdpBroadcastPort();

  //WIFI Station preferences
  void setStationEnabled(bool staEnabled);
  bool getStationEnabled();
  void setStationHostname(const String stationHostname);
  const char * getStationHostname();  
  void setStationSSID(const String staSSID);
  const char * getStationSSID();
  void setStationPassword(const String staPassword);
  const char * getStationPassword();

  //WIFI AP preferences
  void setApSSID(const String apSSID);
  const char * getApSSID();
  void setApPassword(const String apPassword);
  const char * getApPassword();

  //NMEA Preferens
  void setNmeaToSerial(bool value);
  bool isNmeaToSerial();
  void setNmeaToSocket(bool value);
  bool isNmeaToSocket();
  void setNmeaToBluetooth(bool value);
  bool isNmeaToBluetooth();
  void setNmeaToUDP(bool value);
  bool isNmeaToUDP();
};

#endif

