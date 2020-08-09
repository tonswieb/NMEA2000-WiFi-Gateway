#ifndef _N2KPREFERENCES_H_
#define _N2KPREFERENCES_H_

#include <Preferences.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <NMEA2000.h>
#include <functional>

class N2KPreferences {

private:
  const String WIFI_AP_DEFAULT_SSID = "N2K-Bridge";
  const String WIFI_AP_DEFAULT_PASSWORD = "ChangeMe";

  Stream *logger;
  Preferences prefs;
  bool blEnabled;
  bool demoEnabled;

  //NMEA-0183 output receivers
  bool nmeaToSerial;
  bool nmeaToWebSocket;
  bool nmeaToBluetooth;
  bool nmeaToUdp;

  //Input sources for NMEA-0183 output.
  bool nmeaSrcBlGps;
  bool nmeaSrcN2k;
  bool nmeaSrcSerial1;
  bool nmeaSrcSerial2;

  //NMEA200 preferences
  bool nmea2000ToSerial;
  tNMEA2000::tN2kMode nmea2000Mode;

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

  //TODO Ugrade to array so we can have more then one callback
  std::function<void ()> callback;
  std::function<void (bool)> serial1Callback;
  std::function<void (bool)> serial2Callback;


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
  const char *PREF_NMEA2000_TO_SERIAL = "n2kToSerial";
  const char *PREF_NMEA2000_MODE = "nmea2000Mode";
  const char *PREF_NMEA_SRC_BL_GPS_ENABLED = "nmeaSrcBlGps";
  const char *PREF_NMEA_SRC_N2K_ENABLED = "nmeaSrcN2k";
  const char *PREF_NMEA_SRC_SERIAL1_ENABLED = "nmeaSrcSerial1";
  const char *PREF_NMEA_SRC_SERIAL2_ENABLED = "nmeaSrcSerial2";

  N2KPreferences(Stream *logger);
  ~N2KPreferences();

  void registerCallback(std::function<void ()> callback);
  void begin();
  void freeEntries();
  //TODO: Perhaps make this an private function which is called from setters.
  void executeCallbacks();
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

  //NMEA Preferences
  void setNmeaToSerial(bool value);
  bool isNmeaToSerial();
  void setNmeaToSocket(bool value);
  bool isNmeaToSocket();
  void setNmeaToBluetooth(bool value);
  bool isNmeaToBluetooth();
  void setNmeaToUDP(bool value);
  bool isNmeaToUDP();

  void setNmeaSrcBlGPSEnabled(bool value);
  bool isNmeaSrcBlGPSEnabled();
  void setNmeaSrcN2KEnabled(bool value);
  bool isNmeaSrcN2KEnabled();
  void setNmeaSrcSerial1Enabled(bool value);
  //For now support only 1 callback. Should upgrade to support multiple callbacks.
  void setNmeaSrcSerial1Callback(std::function<void (bool)> callback);
  bool isNmeaSrcSerial1Enabled();
  void setNmeaSrcSerial2Enabled(bool value);
  //For now support only 1 callback. Should upgrade to support multiple callbacks.
  void setNmeaSrcSerial2Callback(std::function<void (bool)> callback);
  bool isNmeaSrcSerial2Enabled();

  //NMEA2000 Preferences
  void setNmea2000ToSerial(bool value);
  bool isNmea2000ToSerial();
  void setNmeaMode(tNMEA2000::tN2kMode value);
  tNMEA2000::tN2kMode getNmeaMode();
};

#endif

