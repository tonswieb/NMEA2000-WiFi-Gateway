#include "WifiConnection.h"

WifiConnection::WifiConnection(N2KPreferences *prefs, Stream* logger)
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
        logger->println("[WIFI] Try to connect to WIFI Station.");
        logger->print("[WIFI] SSID: "); logger->println(prefs->getStationSSID());
        logger->print("[WIFI] Hostname: "); logger->println(prefs->getStationHostname());
        logger->print("[WIFI] Password: "); logger->println(prefs->getStationPassword());
        //Try to connect to existing WIFI station (if enabled)
        WiFi.setHostname(prefs->getStationHostname());
        WiFi.mode(WIFI_STA);
        WiFi.begin(prefs->getStationSSID(), prefs->getStationPassword());
        long TimeStamp = millis();
        logger->print("[WIFI] Connecting ");
        while (!WiFi_ok && TimeStamp + 5000 > millis()) //Try to login in know network
        {
            WiFi_ok = !(WiFi.waitForConnectResult() != WL_CONNECTED);
            delay(100);
            logger->print("*");
        }
        if (WiFi_ok)
        {
            logger->println("\r\n[WIFI] Network found and logged in");
            ipLok = WiFi.localIP();
        }
    } else {
        logger->println("[WIFI] Station mode is disabled.");
    }

    if (!WiFi_ok) {
        //if not logged in to known network set up AccesPoint with the same credentials
        logger->println("[WIFI] SETUP WIFI AP.");
        logger->print("[WIFI] SSID: "); logger->println(prefs->getApSSID());
        logger->print("[WIFI] Password: "); logger->println(prefs->getApPassword());

        WiFi.mode(WIFI_AP);
        WiFi.softAP(prefs->getApSSID(), prefs->getApPassword());
        ipLok = WiFi.softAPIP();
    }
    logger->print("[WIFI] Hostname: "); logger->println(WiFi.getMode() == WIFI_STA ? WiFi.getHostname() : WiFi.softAPgetHostname());
    logger->print("[WIFI] IP address: "); logger->println(ipLok);
    ipLok[3] = 255;
    logger->print("[WIFI] UDPport: "); logger->println(prefs->getUdpBroadcastPort());
    logger->println("[WIFI] Setup ready.");
}

void WifiConnection::setup_OTA()
{
    char buf[30];
    byte mac[6];
    logger->print("[OTA] Initializing ");
    WiFi.macAddress(mac);
    sprintf(buf, "%s-%02x:%02x:%02x:%02x:%02x:%02x", prefs->getStationHostname(), mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    logger->println(buf);
    ArduinoOTA.setHostname(buf);
    ArduinoOTA.onStart([&]() {
        /* switch off all processes here!!!!! */
        logger->println();
        logger->println("[OTA] Recieving new firmware now!");
    });
    ArduinoOTA.onEnd([&]() {
        /* do stuff after update here!! */
        logger->println("\n[OTA] Recieving done!");
        logger->println("[OTA] Storing in memory and reboot!");
        logger->println();
    });
    ArduinoOTA.onProgress([&](unsigned int progress, unsigned int total) {
        logger->printf("[OTA] Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) { ESP.restart(); });
    /* setup the OTA server */
    ArduinoOTA.begin();
    logger->println("[OTA] Initialization done!");
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