#ifndef PREFERENCE_REQUESTHANDLER_H
#define PREFERENCE_REQUESTHANDLER_H

#include <WebServer.h>
#include <SPIFFS.h>
#include <detail/RequestHandler.h>
#include <HTTP_Method.h>
#include <ArduinoJson.h>
#include "N2KPreferences.h"
#include "wifi/SuspendableHardwareSerial.h"
#define FILESYSTEM SPIFFS

class PreferenceRequestHandler : public RequestHandler
{
public:
    PreferenceRequestHandler(N2KPreferences *prefs, Stream *logger)
    {
        _prefs = prefs;
        this->logger = logger;
    }

    bool canHandle(HTTPMethod requestMethod, String requestUri) override
    {
        return requestMethod == HTTP_POST || requestMethod == HTTP_GET;
    }

    bool handle(WebServer &server, HTTPMethod requestMethod, String requestUri) override
    {
        if (requestMethod == HTTP_POST) {
            logger->println("Handling Form POST: " + requestUri);
            if (requestUri.equals("/demoSettings")) {
                _prefs->setDemoEnabled(String("on").equals(server.arg(_prefs->PREF_DEMO_ENABLED)));
                _prefs->executeCallbacks();
                server.send(204);
            } else if (requestUri.equals("/nmeaSettings")) {
                _prefs->setNmeaToSerial(String("on").equals(server.arg(_prefs->PREF_NMEA_TO_SERIAL)));
                _prefs->setNmeaToSocket(String("on").equals(server.arg(_prefs->PREF_NMEA_TO_SOCKET)));
                _prefs->setNmeaToBluetooth(String("on").equals(server.arg(_prefs->PREF_NMEA_TO_BL)));
                _prefs->setNmeaToUDP(String("on").equals(server.arg(_prefs->PREF_NMEA_TO_UDP)));
                _prefs->setNmea2000ToSerial(String("on").equals(server.arg(_prefs->PREF_NMEA2000_TO_SERIAL)));
                _prefs->setNmeaMode((tNMEA2000::tN2kMode) server.arg(_prefs->PREF_NMEA2000_MODE).toInt());
                _prefs->setNmeaSrcBlGPSEnabled(String("on").equals(server.arg(_prefs->PREF_NMEA_SRC_BL_GPS_ENABLED)));
                _prefs->setNmeaSrcN2KEnabled(String("on").equals(server.arg(_prefs->PREF_NMEA_SRC_N2K_ENABLED)));
                _prefs->setNmeaSrcSerial1Enabled(String("on").equals(server.arg(_prefs->PREF_NMEA_SRC_SERIAL1_ENABLED)));
                _prefs->setNmeaSrcSerial2Enabled(String("on").equals(server.arg(_prefs->PREF_NMEA_SRC_SERIAL2_ENABLED)));
                _prefs->setNmeaFilter(server.arg(_prefs->PREF_NMEA_TO_FILTER));
                _prefs->setNmea200ReceiveFilter(server.arg(_prefs->PREF_NMEA2000_RECEIVE_FILTER));
                _prefs->executeCallbacks();
                server.send(204);
            } else if (requestUri.equals("/bluetoothSettings")) {
                _prefs->setBlEnabled(String("on").equals(server.arg(_prefs->PREF_BLUETOOTH_ENABLED)));
                _prefs->executeCallbacks();
                server.send(204);
            } else if (requestUri.equals("/wifiSettings")) {
                //TODO: Handle empty String and not a valid integer gracefully. No it just crashes!
                _prefs->setUdpBroadcastPort(server.arg(_prefs->PREF_WIFI_UDP_PORT).toInt());
                _prefs->setStationEnabled(String("on").equals(server.arg(_prefs->PREF_WIFI_STA_ENABLED)));
                if (_prefs->getStationEnabled()) {
                    _prefs->setStationHostname(server.arg(_prefs->PREF_WIFI_STA_HOSTNAME));
                    _prefs->setStationSSID(server.arg(_prefs->PREF_WIFI_STA_SSID));
                    _prefs->setStationPassword(server.arg(_prefs->PREF_WIFI_STA_PASSWORD));
                } else {
                    _prefs->setApSSID(server.arg(_prefs->PREF_WIFI_AP_SSID));
                    _prefs->setApPassword(server.arg(_prefs->PREF_WIFI_AP_PASSWORD));
                }
                _prefs->executeCallbacks();
                server.send(204);
            } else {
                logger->println("Received unknown form POST: " + requestUri);
                server.send(404, "text/plain", "FileNotFound");          
            }
        } else {
            if (requestUri.equals("/load")) {
                StaticJsonDocument<600> doc;
                doc[_prefs->PREF_BLUETOOTH_ENABLED] =  _prefs->isBlEnabled();
                doc[_prefs->PREF_DEMO_ENABLED] =  _prefs->isDemoEnabled();
                doc[_prefs->PREF_WIFI_STA_ENABLED] =  _prefs->getStationEnabled();
                doc[_prefs->PREF_WIFI_UDP_PORT] =  String(_prefs->getUdpBroadcastPort());
                doc[_prefs->PREF_WIFI_STA_HOSTNAME] =  _prefs->getStationHostname();
                doc[_prefs->PREF_WIFI_STA_SSID] =  _prefs->getStationSSID();
                doc[_prefs->PREF_WIFI_STA_PASSWORD] =  _prefs->getStationPassword();
                doc[_prefs->PREF_WIFI_AP_SSID] =  _prefs->getApSSID();
                doc[_prefs->PREF_WIFI_AP_PASSWORD] =  _prefs->getApPassword();
                doc[_prefs->PREF_NMEA_TO_SERIAL] =  _prefs->isNmeaToSerial();
                doc[_prefs->PREF_NMEA_TO_SOCKET] =  _prefs->isNmeaToSocket();
                doc[_prefs->PREF_NMEA_TO_BL] =  _prefs->isNmeaToBluetooth();
                doc[_prefs->PREF_NMEA_TO_UDP] =  _prefs->isNmeaToUDP();
                doc[_prefs->PREF_NMEA2000_TO_SERIAL] =  _prefs->isNmea2000ToSerial();
                doc[_prefs->PREF_NMEA_SRC_BL_GPS_ENABLED] =  _prefs->isNmeaSrcBlGPSEnabled();
                doc[_prefs->PREF_NMEA_SRC_N2K_ENABLED] =  _prefs->isNmeaSrcN2KEnabled();
                doc[_prefs->PREF_NMEA_SRC_SERIAL1_ENABLED] =  _prefs->isNmeaSrcSerial1Enabled();
                doc[_prefs->PREF_NMEA_SRC_SERIAL2_ENABLED] =  _prefs->isNmeaSrcSerial2Enabled();
                doc[_prefs->PREF_NMEA_TO_FILTER] =  _prefs->getNmeaFilter();
                doc[_prefs->PREF_NMEA2000_RECEIVE_FILTER] =  _prefs->getNmea200ReceiveFilter();

                doc["nmea2000Mode4"] =  _prefs->getNmeaMode() == tNMEA2000::N2km_ListenAndSend;
                doc["nmea2000Mode2"] =  _prefs->getNmeaMode() == tNMEA2000::N2km_ListenAndNode;
                doc["nmea2000Mode0"] =  _prefs->getNmeaMode() == tNMEA2000::N2km_ListenOnly;
                doc["nmea2000Mode1"] =  _prefs->getNmeaMode() == tNMEA2000::N2km_NodeOnly;
                doc["nmea2000Mode3"] =  _prefs->getNmeaMode() == tNMEA2000::N2km_SendOnly;
                char buffer[600];
                serializeJson(doc, buffer);
                server.send(200,"application/json",buffer);
            } else if (requestUri.equals("/reboot")) {
                server.send(200);
                ESP.restart();
            } else if (requestUri.equals("/reset")) {
                server.send(200);
                _prefs->reset();
            } else if (requestUri.equals("/factoryReset")) {
                server.send(200);
                _prefs->factoryReset();
            } else if (requestUri.equals("/favicon.ico")) {
                server.send(404, "text/plain", "FileNotFound");
            } else if (!handleFileRead(server)) {
                server.send(404, "text/plain", "FileNotFound");
            }
        }
        return true;
    }

protected:
    N2KPreferences *_prefs;
    Stream *logger;

    String getContentType(WebServer &server, String filename)
    {
        if (server.hasArg("download"))
        {
            return "application/octet-stream";
        }
        else if (filename.endsWith(".htm"))
        {
            return "text/html";
        }
        else if (filename.endsWith(".html"))
        {
            return "text/html";
        }
        else if (filename.endsWith(".css"))
        {
            return "text/css";
        }
        else if (filename.endsWith(".js"))
        {
            return "application/javascript";
        }
        else if (filename.endsWith(".png"))
        {
            return "image/png";
        }
        else if (filename.endsWith(".gif"))
        {
            return "image/gif";
        }
        else if (filename.endsWith(".jpg"))
        {
            return "image/jpeg";
        }
        else if (filename.endsWith(".ico"))
        {
            return "image/x-icon";
        }
        else if (filename.endsWith(".xml"))
        {
            return "text/xml";
        }
        else if (filename.endsWith(".pdf"))
        {
            return "application/x-pdf";
        }
        else if (filename.endsWith(".zip"))
        {
            return "application/x-zip";
        }
        else if (filename.endsWith(".gz"))
        {
            return "application/x-gzip";
        }
        return "text/plain";
    }

    bool exists(String path)
    {
        bool yes = false;
        File file = FILESYSTEM.open(path, "r");
        if (!file.isDirectory())
        {
            yes = true;
        }
        file.close();
        return yes;
    }

    bool handleFileRead(WebServer &server)
    {
        String path = server.uri();
        logger->println("handleFileRead: " + path);
        if (path.endsWith("/")) {
            path += "index.html";
        }

        String contentType = getContentType(server, path);
        String pathWithGz = path + ".gz";
        if (exists(pathWithGz) || exists(path))
        {
            if (exists(pathWithGz))
            {
                path += ".gz";
            }
            sendFileContent(server, path, contentType);
            return true;
        }
        return false;
    }

    void sendFileContent(WebServer &server, String path, String contentType)
    {
        File file = FILESYSTEM.open(path, "r");
        server.streamFile(file, contentType);
        file.close();
    }

    String getVar(const char *pref)
    {
        return "$" + String(pref);
    }
};
#endif //REQUESTHANDLERSIMPL_H
