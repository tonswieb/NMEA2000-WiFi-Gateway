#include "N2KPreferences.h"

N2KPreferences::N2KPreferences() {
}

N2KPreferences::~N2KPreferences() {
    end();
}

void N2KPreferences::begin() {

    prefs.begin(PREF_SPACE);
    blEnabled = prefs.getBool(PREF_BLUETOOTH_ENABLED,false);
    demoEnabled = prefs.getBool(PREF_DEMO_ENABLED, false);
    udpPort = prefs.getUInt(PREF_WIFI_UDP_PORT, 9876);
    staEnabled = prefs.getBool(PREF_WIFI_STA_ENABLED, false);
    stationHostname = prefs.getString(PREF_WIFI_STA_HOSTNAME);
    staSSID = prefs.getString(PREF_WIFI_STA_SSID);
    staPassword = prefs.getString(PREF_WIFI_STA_PASSWORD);
    apSSID = prefs.getString(PREF_WIFI_AP_SSID, WIFI_AP_DEFAULT_SSID);
    apPassword = prefs.getString(PREF_WIFI_AP_PASSWORD, WIFI_AP_DEFAULT_PASSWORD);
}

void N2KPreferences::end() {
    prefs.end();
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
