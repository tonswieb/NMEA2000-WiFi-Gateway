#include "WifiConnection.h"

WifiConnection::WifiConnection()
{

    ResetWiFiSettingsOnNvs();
    delay(1000);
    setupWIFI();
    setup_OTA();
}

void WifiConnection::ResetWiFiSettingsOnNvs()
{
    int err;
    err = nvs_flash_init();
    Serial.println("nvs_flash_init: " + err);
    err = nvs_flash_erase();
    Serial.println("nvs_flash_erase: " + err);
}

void WifiConnection::setupWIFI()
{
    long TimeStamp;
    bool WiFi_ok = false;

    Serial.println("[SETUP] Booting");

    WiFi.setHostname(host);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    TimeStamp = millis();
    Serial.print("[SETUP] ");
    while (!WiFi_ok && TimeStamp + 5000 > millis()) //Try to login in know network
    {
        WiFi_ok = !(WiFi.waitForConnectResult() != WL_CONNECTED);
        delay(100);
        Serial.print("*");
    }
    if (WiFi_ok)
    {
        Serial.println("\r\n[SETUP] Network found and logged in");
        ipLok = WiFi.localIP();
    }

    else
    {
        Serial.println("\r\n[SETUP] Setup AP");
        WiFi.mode(WIFI_AP);
        WiFi.softAP(ssid, password); // if not logged in to known network set up AccesPoint with the same credentials
        ipLok = WiFi.softAPIP();
    }
    ipLok[3] = 255;
    Serial.print("[SETUP] IP address: ");
    Serial.println(ipLok);
    Serial.println("[SETUP] Ready");
}

void WifiConnection::setup_OTA()
{
    char buf[30];
    byte mac[6];
    Serial.println();
    Serial.print("[SETUP] OTA...");
    WiFi.macAddress(mac);
    sprintf(buf, "%s-%02x:%02x:%02x:%02x:%02x:%02x", host, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    Serial.print(buf);
    ArduinoOTA.setHostname(buf);
    ArduinoOTA.onStart([]() {
        /* switch off all processes here!!!!! */
        Serial.println();
        Serial.println("Recieving new firmware now!");
    });
    ArduinoOTA.onEnd([]() {
        /* do stuff after update here!! */
        Serial.println("\nRecieving done!");
        Serial.println("Storing in memory and reboot!");
        Serial.println();
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) { ESP.restart(); });
    /* setup the OTA server */
    ArduinoOTA.begin();
    Serial.println("...done!");
}

void WifiConnection::sendUdpPackage(char *buf)
{

    udp.beginPacket(ipLok, port);
    udp.println(buf);
    udp.endPacket();
}