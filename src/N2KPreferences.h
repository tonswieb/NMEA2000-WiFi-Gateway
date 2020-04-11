#ifndef _N2KPREFERENCES_H_
#define _N2KPREFERENCES_H_

#include <Preferences.h>

class N2KPreferences {

private:
  const String WIFI_AP_DEFAULT_SSID = "N2K-Bridge";
  const String WIFI_AP_DEFAULT_PASSWORD = "ChangeMe";

  Preferences prefs;
  volatile bool blEnabled;
  volatile bool demoEnabled;

  //TODO: Move WIFI preferences to own class
  //WIFI General preferences
  int udpPort;

  //WIFI Station preferences
  volatile bool staEnabled;
  String stationHostname;
  String staSSID;
  String staPassword;
  
  //WIFI AP preferences
  String apSSID;
  String apPassword;

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

  N2KPreferences();
  ~N2KPreferences();

  void begin();
  void end();
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
};

#endif

