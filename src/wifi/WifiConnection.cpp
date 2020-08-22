#include "WifiConnection.h"

WifiConnection::WifiConnection(N2KPreferences *prefs, Logger* logger)
{
    this->prefs = prefs;
    this->logger = logger;
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
        info("[WIFI] Try to connect to WIFI Station.");
        info("[WIFI] SSID: %s",prefs->getStationSSID());
        info("[WIFI] Hostname: %s",prefs->getStationHostname());
        info("[WIFI] Password: %s",prefs->getStationPassword());
        //Try to connect to existing WIFI station (if enabled)
        WiFi.setHostname(prefs->getStationHostname());
        WiFi.mode(WIFI_STA);
        WiFi.begin(prefs->getStationSSID(), prefs->getStationPassword());
        long TimeStamp = millis();
        logger->getStream()->print("[WIFI] Connecting ");
        while (!WiFi_ok && TimeStamp + 5000 > millis()) //Try to login in know network
        {
            WiFi_ok = !(WiFi.waitForConnectResult() != WL_CONNECTED);
            delay(100);
            logger->getStream()->print("*");
        }
        if (WiFi_ok)
        {
            info("");
            info("[WIFI] Network found and logged in");
            ipLok = WiFi.localIP();
        }
    } else {
        info("[WIFI] Station mode is disabled.");
    }

    if (!WiFi_ok) {
        //if not logged in to known network set up AccesPoint with the same credentials
        info("[WIFI] SETUP WIFI AP.");
        info("[WIFI] SSID: %s",prefs->getApSSID());
        info("[WIFI] Password: %s",prefs->getApPassword());

        WiFi.mode(WIFI_AP);
        WiFi.softAP(prefs->getApSSID(), prefs->getApPassword());
        ipLok = WiFi.softAPIP();
    }
    info("[WIFI] Hostname: %s",WiFi.getMode() == WIFI_STA ? WiFi.getHostname() : WiFi.softAPgetHostname());
    info("[WIFI] IP address: %s",ipLok);
    ipLok[3] = 255;
    info("[WIFI] UDPport: %u",prefs->getUdpBroadcastPort());
    info("[WIFI] Setup ready.");
}

void WifiConnection::setup_OTA()
{
    char buf[30];
    byte mac[6];
    info("[OTA] Initializing ");
    WiFi.macAddress(mac);
    sprintf(buf, "%s-%02x:%02x:%02x:%02x:%02x:%02x", prefs->getStationHostname(), mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    info(buf);
    ArduinoOTA.setHostname(buf);
    ArduinoOTA.onStart([&]() {
        /* switch off all processes here!!!!! */
        info("[OTA] Recieving new firmware now!");
    });
    ArduinoOTA.onEnd([&]() {
        /* do stuff after update here!! */
        info("[OTA] Recieving done!");
        info("[OTA] Storing in memory and reboot!");
    });
    ArduinoOTA.onProgress([&](unsigned int progress, unsigned int total) {
        info("[OTA] Progress: %u", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) { ESP.restart(); });
    /* setup the OTA server */
    ArduinoOTA.begin();
    info("[OTA] Initialization done!");
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