#include "WifiConnection.h"

WifiConnection::WifiConnection(N2KPreferences *prefs)
{
    this->prefs = prefs;
}

WifiConnection::~WifiConnection()
{
    end();
}

void WifiConnection::begin() {

    setupWIFI();
    setup_OTA();
    udp.begin(9877);
}

void WifiConnection::end() {
    
}

void WifiConnection::setupWIFI()
{
    bool WiFi_ok = false;
    if (prefs->getStationEnabled()) {
        Serial.println("[WIFI] Try to connect to WIFI Station.");
        Serial.print("[WIFI] SSID: "); Serial.println(prefs->getStationSSID());
        Serial.print("[WIFI] Hostname: "); Serial.println(prefs->getStationHostname());
        Serial.print("[WIFI] Password: "); Serial.println(prefs->getStationPassword());
        //Try to connect to existing WIFI station (if enabled)
        WiFi.setHostname(prefs->getStationHostname());
        WiFi.mode(WIFI_STA);
        WiFi.begin(prefs->getStationSSID(), prefs->getStationPassword());
        long TimeStamp = millis();
        Serial.print("[WIFI] Connecting ");
        while (!WiFi_ok && TimeStamp + 5000 > millis()) //Try to login in know network
        {
            WiFi_ok = !(WiFi.waitForConnectResult() != WL_CONNECTED);
            delay(100);
            Serial.print("*");
        }
        if (WiFi_ok)
        {
            Serial.println("\r\n[WIFI] Network found and logged in");
            ipLok = WiFi.localIP();
        }
    } else {
        Serial.println("[WIFI] Station mode is disabled.");
    }

    if (!WiFi_ok) {
        //if not logged in to known network set up AccesPoint with the same credentials
        Serial.println("[WIFI] SETUP WIFI AP.");
        Serial.print("[WIFI] SSID: "); Serial.println(prefs->getApSSID());
        Serial.print("[WIFI] Password: "); Serial.println(prefs->getApPassword());

        WiFi.mode(WIFI_AP);
        WiFi.softAP(prefs->getApSSID(), prefs->getApPassword());
        ipLok = WiFi.softAPIP();
    }
    Serial.print("[WIFI] Hostname: "); Serial.println(WiFi.getMode() == WIFI_STA ? WiFi.getHostname() : WiFi.softAPgetHostname());
    Serial.print("[WIFI] IP address: "); Serial.println(ipLok);
    ipLok[3] = 255;
    Serial.print("[WIFI] UDPport: "); Serial.println(prefs->getUdpBroadcastPort());
    Serial.println("[WIFI] Setup ready.");
}

void WifiConnection::setup_OTA()
{
    char buf[30];
    byte mac[6];
    Serial.print("[OTA] Initializing ");
    WiFi.macAddress(mac);
    sprintf(buf, "%s-%02x:%02x:%02x:%02x:%02x:%02x", prefs->getStationHostname(), mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    Serial.println(buf);
    ArduinoOTA.setHostname(buf);
    ArduinoOTA.onStart([]() {
        /* switch off all processes here!!!!! */
        Serial.println();
        Serial.println("[OTA] Recieving new firmware now!");
    });
    ArduinoOTA.onEnd([]() {
        /* do stuff after update here!! */
        Serial.println("\n[OTA] Recieving done!");
        Serial.println("[OTA] Storing in memory and reboot!");
        Serial.println();
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("[OTA] Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) { ESP.restart(); });
    /* setup the OTA server */
    ArduinoOTA.begin();
    Serial.println("[OTA] Initialization done!");
}

void WifiConnection::sendUdpPackage(char *buf)
{
    if (prefs->isNmeaToUDP()) {
        udp.beginPacket(ipLok, prefs->getUdpBroadcastPort());
        udp.println(buf);
        udp.endPacket();
    }
}

Stream* WifiConnection::getUdpPackageStream() {
    return &udpStream;
}

void WifiConnection::loop() {
  ArduinoOTA.handle();
}