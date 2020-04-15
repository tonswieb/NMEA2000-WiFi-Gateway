#ifndef PREFERENCE_REQUESTHANDLER_H
#define PREFERENCE_REQUESTHANDLER_H

#include <WebServer.h>
#include <SPIFFS.h>
#include <detail/RequestHandler.h>
#include <HTTP_Method.h>
#include "N2KPreferences.h"
#include "NMEA2000WiFiGateway.h"
#define FILESYSTEM SPIFFS

class PreferenceRequestHandler : public RequestHandler
{
public:
    PreferenceRequestHandler(N2KPreferences *prefs)
    {
        _prefs = prefs;
    }

    bool canHandle(HTTPMethod requestMethod, String requestUri) override
    {
        return requestMethod == HTTP_POST || requestMethod == HTTP_GET;
    }

    bool handle(WebServer &server, HTTPMethod requestMethod, String requestUri) override
    {
        if (requestMethod == HTTP_POST) {
            Serial.println("Handling Form POST: " + requestUri);
            if (requestUri.equals("/demoSettings")) {
                _prefs->setDemoEnabled(String("on").equals(server.arg(_prefs->PREF_DEMO_ENABLED)));
                server.send(204);
            } else if (requestUri.equals("/nmeaSettings")) {
                _prefs->setNmeaToSerial(String("on").equals(server.arg(_prefs->PREF_NMEA_TO_SERIAL)));
                _prefs->setNmeaToSocket(String("on").equals(server.arg(_prefs->PREF_NMEA_TO_SOCKET)));
                _prefs->setNmeaToBluetooth(String("on").equals(server.arg(_prefs->PREF_NMEA_TO_BL)));
                _prefs->setNmeaToUDP(String("on").equals(server.arg(_prefs->PREF_NMEA_TO_UDP)));
                _prefs->setNmea2000ToSerial(String("on").equals(server.arg(_prefs->PREF_NMEA2000_TO_SERIAL)));
                _prefs->setNmeaMode((tNMEA2000::tN2kMode) server.arg(_prefs->PREF_NMEA2000_MODE).toInt());
                server.send(204);
            } else if (requestUri.equals("/bluetoothSettings")) {
                _prefs->setBlEnabled(String("on").equals(server.arg(_prefs->PREF_BLUETOOTH_ENABLED)));
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
                server.send(204);
            } else {
                Serial.println("Received unknown form POST: " + requestUri);
                server.send(404, "text/plain", "FileNotFound");          
            }
        } else {
            if (requestUri.equals("/reboot")) {
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
        Serial.println("handleFileRead: " + path);
        if (path.endsWith("/")) {
            path += "index.html";
        }

        String pathWithGz = path + ".gz";
        if (exists(pathWithGz) || exists(path))
        {
            if (exists(pathWithGz))
            {
                path += ".gz";
            }
            sendFileContent(server, path);
            return true;
        }
        return false;
    }

    void sendFileContent(WebServer &server, String path)
    {
        File file = FILESYSTEM.open(path, "r");
        if (path.endsWith("html")) {
            String content = file.readString();
            content.replace(getVar(_prefs->PREF_BLUETOOTH_ENABLED), _prefs->isBlEnabled() ? "checked" : "");
            content.replace(getVar(_prefs->PREF_DEMO_ENABLED), _prefs->isDemoEnabled() ? "checked" : "");
            content.replace(getVar(_prefs->PREF_WIFI_STA_ENABLED), _prefs->getStationEnabled() ? "checked" : "");
            content.replace(getVar(_prefs->PREF_WIFI_UDP_PORT), String(_prefs->getUdpBroadcastPort()));
            content.replace(getVar(_prefs->PREF_WIFI_STA_HOSTNAME), _prefs->getStationHostname());
            content.replace(getVar(_prefs->PREF_WIFI_STA_SSID), _prefs->getStationSSID());
            content.replace(getVar(_prefs->PREF_WIFI_STA_PASSWORD), _prefs->getStationPassword());
            content.replace(getVar(_prefs->PREF_WIFI_AP_SSID), _prefs->getApSSID());
            content.replace(getVar(_prefs->PREF_WIFI_AP_PASSWORD), _prefs->getApPassword());
            content.replace(getVar(_prefs->PREF_NMEA_TO_SERIAL), _prefs->isNmeaToSerial() ? "checked" : "");
            content.replace(getVar(_prefs->PREF_NMEA_TO_SOCKET), _prefs->isNmeaToSocket() ? "checked" : "");
            content.replace(getVar(_prefs->PREF_NMEA_TO_BL), _prefs->isNmeaToBluetooth() ? "checked" : "");
            content.replace(getVar(_prefs->PREF_NMEA_TO_UDP), _prefs->isNmeaToUDP() ? "checked" : "");
            content.replace(getVar(_prefs->PREF_NMEA2000_TO_SERIAL), _prefs->isNmea2000ToSerial() ? "checked" : "");
            //TODO: Factor out the duplication into a function
            content.replace(getVar("nmea2000Mode" + tNMEA2000::N2km_ListenAndSend), _prefs->getNmeaMode() == tNMEA2000::N2km_ListenAndSend ? "selected" : "");
            content.replace(getVar("nmea2000Mode" + tNMEA2000::N2km_ListenAndNode), _prefs->getNmeaMode() == tNMEA2000::N2km_ListenAndNode ? "selected" : "");
            content.replace(getVar("nmea2000Mode" + tNMEA2000::N2km_ListenOnly), _prefs->getNmeaMode() == tNMEA2000::N2km_ListenOnly ? "selected" : "");
            content.replace(getVar("nmea2000Mode" + tNMEA2000::N2km_NodeOnly), _prefs->getNmeaMode() == tNMEA2000::N2km_NodeOnly ? "selected" : "");
            content.replace(getVar("nmea2000Mode" + tNMEA2000::N2km_SendOnly), _prefs->getNmeaMode() == tNMEA2000::N2km_SendOnly ? "selected" : "");
            server.send(200, "text/html", content);
        } else {
            String contentType = getContentType(server, path);
            server.streamFile(file, contentType);
        }
        file.close();
    }

    String getVar(const char *pref)
    {
        return "$" + String(pref);
    }
};
#endif //REQUESTHANDLERSIMPL_H
