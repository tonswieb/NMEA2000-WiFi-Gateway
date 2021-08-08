#include "N2KPreferences.h"

N2KPreferences::N2KPreferences(Logger *logger) {
    this->logger = logger;
}

N2KPreferences::~N2KPreferences() {
    end();
}

void N2KPreferences::registerCallback(std::function<void ()> callback) {
    this->callback = callback;
}

void N2KPreferences::executeCallbacks() {
    if (callback) {
        callback();
    }
}

void N2KPreferences::begin() {

    prefs.begin(PREF_SPACE);
    init();
}

void N2KPreferences::freeEntries() {

    info("[PREF] Free Entries: %u",prefs.freeEntries());
}


void N2KPreferences::end() {
    prefs.end();
}

void N2KPreferences::init() {

    //Retrieve settings, use default if missing and store
    //directly to ensure that everything fits in NVS.
    setBlEnabled(prefs.getBool(PREF_BLUETOOTH_ENABLED,false));
    setLogLevel(prefs.getUInt(PREF_LOG_LEVEL,DEBUG_LEVEL_WARN));
    setNmeaSrcBlGPSEnabled(prefs.getBool(PREF_NMEA_SRC_BL_GPS_ENABLED,false));
    setNmeaSrcN2KEnabled(prefs.getBool(PREF_NMEA_SRC_N2K_ENABLED,true));
    setNmeaSrcSerial1Enabled(prefs.getBool(PREF_NMEA_SRC_SERIAL1_ENABLED,true));
    setNmeaSrcSerial2Enabled(prefs.getBool(PREF_NMEA_SRC_SERIAL2_ENABLED,true));
    setDemoEnabled(prefs.getBool(PREF_DEMO_ENABLED, false));
    setUdpBroadcastPort(prefs.getUInt(PREF_WIFI_UDP_PORT, 9876));
    setStationEnabled(prefs.getBool(PREF_WIFI_STA_ENABLED, false));
    setStationHostname(prefs.getString(PREF_WIFI_STA_HOSTNAME));
    setStationSSID(prefs.getString(PREF_WIFI_STA_SSID));
    setStationPassword(prefs.getString(PREF_WIFI_STA_PASSWORD));
    setApSSID(prefs.getString(PREF_WIFI_AP_SSID, WIFI_AP_DEFAULT_SSID));
    setApPassword(prefs.getString(PREF_WIFI_AP_PASSWORD, WIFI_AP_DEFAULT_PASSWORD));
    setNmeaToSerial(prefs.getBool(PREF_NMEA_TO_SERIAL,false));
    setNmeaToSocket(prefs.getBool(PREF_NMEA_TO_SOCKET, false));
    setNmeaToBluetooth(prefs.getBool(PREF_NMEA_TO_BL,true));
    setNmeaToUDP(prefs.getBool(PREF_NMEA_TO_UDP,true));
    setNmea2000ToSerial(prefs.getBool(PREF_NMEA2000_TO_SERIAL,false));
    setNmeaMode((tNMEA2000::tN2kMode) prefs.getUChar(PREF_NMEA2000_MODE, tNMEA2000::N2km_ListenOnly));
    setNmea200ReceiveFilter(prefs.getString(PREF_NMEA2000_RECEIVE_FILTER, NMEA2000_RECEIVE_FILTER));
}

void N2KPreferences::reset() {
    prefs.clear();
    ESP.restart();
}

void N2KPreferences::factoryReset() {
    nvs_flash_erase();
    ESP.restart();
}

void N2KPreferences::setBlEnabled(bool value) {
    prefs.putBool(PREF_BLUETOOTH_ENABLED,value);
    this->blEnabled = value;
}

void N2KPreferences::setDemoEnabled(bool value) {
    prefs.putBool(PREF_DEMO_ENABLED,value);
    this->demoEnabled = value;
 }

bool N2KPreferences::isBlEnabled() {
    return blEnabled;
}

bool N2KPreferences::isDemoEnabled() {
    return demoEnabled;
}

void N2KPreferences::setLogLevel(int logLevel) {
    prefs.putUInt(PREF_LOG_LEVEL, logLevel);
    this->logLevel = logLevel;
    if (logLevelCallback) {
        logLevelCallback(logLevel);
    }
}

int N2KPreferences::getLogLevel() {
    return logLevel;
}

void N2KPreferences::setLogLevelCallBack(std::function<void (int)> callback) {
    logLevelCallback = callback;
}


//WIFI General
void N2KPreferences::setUdpBroadcastPort(int port) {
    prefs.putUInt(PREF_WIFI_UDP_PORT, port);
    this->udpPort = port;
}
int N2KPreferences::getUdpBroadcastPort() {
    return udpPort;
}

//WIFI Station preferences
void N2KPreferences::setStationEnabled(bool staEnabled) {
    prefs.putBool(PREF_WIFI_STA_ENABLED, staEnabled);
    this->staEnabled = staEnabled;
}
bool N2KPreferences::getStationEnabled() {
    return staEnabled;
}
void N2KPreferences::setStationHostname(const String stationHostname) {
    prefs.putString(PREF_WIFI_STA_HOSTNAME, stationHostname);
    this->stationHostname = stationHostname;
}
const char * N2KPreferences::getStationHostname() {
    return stationHostname.c_str();
}
void N2KPreferences::setStationSSID(const String staSSID) {
    prefs.putString(PREF_WIFI_STA_SSID, staSSID);
    this->staSSID = staSSID;
}
const char * N2KPreferences::getStationSSID() {
    return staSSID.c_str();
}
void N2KPreferences::setStationPassword(const String staPassword) {
    prefs.putString(PREF_WIFI_STA_PASSWORD, staPassword);
    this->staPassword = staPassword;
}
const char * N2KPreferences::getStationPassword() {
    return staPassword.c_str();
}

//WIFI AP preferences
void N2KPreferences::setApSSID(const String apSSID) {
    prefs.putString(PREF_WIFI_AP_SSID, apSSID);
    this->apSSID = apSSID;
}
const char * N2KPreferences::N2KPreferences::getApSSID() {
    return apSSID.c_str();
}
void N2KPreferences::setApPassword(const String apPassword) {
    prefs.putString(PREF_WIFI_AP_PASSWORD, apPassword);
    this->apPassword = apPassword;
}
const char * N2KPreferences::getApPassword() {
    return apPassword.c_str();
}

//NMEA Preferens
void N2KPreferences::setNmeaToSerial(bool value) {
    prefs.putBool(PREF_NMEA_TO_SERIAL, value);
    nmeaToSerial = value;
}
bool N2KPreferences::isNmeaToSerial() {
    return nmeaToSerial;
}
void N2KPreferences::setNmeaToSocket(bool value) {
    prefs.putBool(PREF_NMEA_TO_SOCKET, value);
    nmeaToWebSocket = value;
}
bool N2KPreferences::isNmeaToSocket() {
    return nmeaToWebSocket;
}
void N2KPreferences::setNmeaToBluetooth(bool value) {
    prefs.putBool(PREF_NMEA_TO_BL, value);
    nmeaToBluetooth = value;
}
bool N2KPreferences::isNmeaToBluetooth() {
    return nmeaToBluetooth;
}
void N2KPreferences::setNmeaToUDP(bool value) {
    prefs.putBool(PREF_NMEA_TO_UDP, value);
    nmeaToUdp = value;
}
bool N2KPreferences::isNmeaToUDP() {
    return nmeaToUdp;
}
void N2KPreferences::setNmea2000ToSerial(bool value) {
    prefs.putBool(PREF_NMEA2000_TO_SERIAL, value);
    nmea2000ToSerial = value;
}
bool N2KPreferences::isNmea2000ToSerial() {
    return nmea2000ToSerial;    
}
void N2KPreferences::setNmeaMode(tNMEA2000::tN2kMode value) {
    prefs.putUChar(PREF_NMEA2000_MODE, value);
    nmea2000Mode = value;
}
tNMEA2000::tN2kMode N2KPreferences::getNmeaMode() {
    return nmea2000Mode;
}

void N2KPreferences::setNmeaSrcBlGPSEnabled(bool value) {
    prefs.putBool(PREF_NMEA_SRC_BL_GPS_ENABLED, value);
    nmeaSrcBlGps = value;
}
bool N2KPreferences::isNmeaSrcBlGPSEnabled() {
    return nmeaSrcBlGps;
}
void N2KPreferences::setNmeaSrcN2KEnabled(bool value) {
    prefs.putBool(PREF_NMEA_SRC_N2K_ENABLED, value);
    nmeaSrcN2k = value;
}
bool N2KPreferences::isNmeaSrcN2KEnabled() {
    return nmeaSrcN2k;
}
void N2KPreferences::setNmeaSrcSerial1Enabled(bool value) {
    prefs.putBool(PREF_NMEA_SRC_SERIAL1_ENABLED, value);
    nmeaSrcSerial1 = value;
    if (serial1Callback) {
        serial1Callback(value);
    }
}
bool N2KPreferences::isNmeaSrcSerial1Enabled() {
    return nmeaSrcSerial1;
}
void N2KPreferences::setNmeaSrcSerial1Callback(std::function<void (bool)> callback) {
    serial1Callback = callback;
}
void N2KPreferences::setNmeaSrcSerial2Enabled(bool value) {
    prefs.putBool(PREF_NMEA_SRC_SERIAL2_ENABLED, value);
    nmeaSrcSerial2 = value;
    if (serial2Callback) {
        serial2Callback(value);
    }
}
bool N2KPreferences::isNmeaSrcSerial2Enabled() {
    return nmeaSrcSerial2;
}
void N2KPreferences::setNmeaSrcSerial2Callback(std::function<void (bool)> callback) {
    serial2Callback = callback;
}

void N2KPreferences::setNmeaFilter(const String nmeaFilter) {
    prefs.putString(PREF_NMEA_TO_FILTER, nmeaFilter);
    this->nmeaFilter = nmeaFilter;
}
const char * N2KPreferences::getNmeaFilter() {
    return nmeaFilter.c_str();
}

void N2KPreferences::setNmea200ReceiveFilter(const String nmea200ReceiveFilter) {
    prefs.putString(PREF_NMEA2000_RECEIVE_FILTER, nmea200ReceiveFilter);
    this->nmea200ReceiveFilter = nmea200ReceiveFilter;
}

const char * N2KPreferences::getNmea200ReceiveFilter() {
    return nmea200ReceiveFilter.c_str();
}

void N2KPreferences::setNmea2000Callback(std::function<void ()> callback) {
    this->nmea2000Callback = callback;
}

void N2KPreferences::executeNmea2000Callback() {
    if (nmea2000Callback) {
        nmea2000Callback();
    }
}