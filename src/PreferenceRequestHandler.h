#ifndef PREFERENCE_REQUESTHANDLER_H
#define PREFERENCE_REQUESTHANDLER_H

#include <WebServer.h>
#include <SPIFFS.h>
#include <detail/RequestHandler.h>
#include <HTTP_Method.h>
#include "N2KPreferences.h"
#define FILESYSTEM SPIFFS


class PreferenceRequestHandler : public RequestHandler {
public:
    PreferenceRequestHandler(N2KPreferences *prefs) {
        _prefs = prefs;
    }

    bool canHandle(HTTPMethod requestMethod, String requestUri) override  {
        return true;
    }

    bool handle(WebServer& server, HTTPMethod requestMethod, String requestUri) override {
        if (server.hasArg("handleForm")) {
            Serial.println("Handle Form");
            _prefs->setBlEnabled(server.hasArg(_prefs->PREF_BLUETOOTH_ENABLED));
            _prefs->setDemoEnabled(server.hasArg(_prefs->PREF_DEMO_ENABLED));
            _prefs->setStationEnabled(server.hasArg(_prefs->PREF_WIFI_STA_ENABLED));
            _prefs->setStationHostname(server.arg(_prefs->PREF_WIFI_STA_HOSTNAME));
            _prefs->setStationSSID(server.arg(_prefs->PREF_WIFI_STA_SSID));
            _prefs->setStationPassword(server.arg(_prefs->PREF_WIFI_STA_PASSWORD));
            _prefs->setApSSID(server.arg(_prefs->PREF_WIFI_AP_SSID));
            _prefs->setApPassword(server.arg(_prefs->PREF_WIFI_AP_PASSWORD));
            //TODO: Handle empty String and not a valid integer gracefully. No it just crashes!
            _prefs->setUdpBroadcastPort(server.arg(_prefs->PREF_WIFI_UDP_PORT).toInt());
            ESP.restart();
        }
        if (!handleFileRead(server)) {
            server.send(404, "text/plain", "FileNotFound");
        }
        return true;
    }

protected:
    N2KPreferences *_prefs;

    String getContentType(WebServer& server, String filename) {
    if (server.hasArg("download")) {
        return "application/octet-stream";
    } else if (filename.endsWith(".htm")) {
        return "text/html";
    } else if (filename.endsWith(".html")) {
        return "text/html";
    } else if (filename.endsWith(".css")) {
        return "text/css";
    } else if (filename.endsWith(".js")) {
        return "application/javascript";
    } else if (filename.endsWith(".png")) {
        return "image/png";
    } else if (filename.endsWith(".gif")) {
        return "image/gif";
    } else if (filename.endsWith(".jpg")) {
        return "image/jpeg";
    } else if (filename.endsWith(".ico")) {
        return "image/x-icon";
    } else if (filename.endsWith(".xml")) {
        return "text/xml";
    } else if (filename.endsWith(".pdf")) {
        return "application/x-pdf";
    } else if (filename.endsWith(".zip")) {
        return "application/x-zip";
    } else if (filename.endsWith(".gz")) {
        return "application/x-gzip";
    }
    return "text/plain";
    }

    bool exists(String path) {
        bool yes = false;
        File file = FILESYSTEM.open(path, "r");
        if(!file.isDirectory()){
            yes = true;
        }
        file.close();
        return yes;
    }

    bool handleFileRead(WebServer& server) {
    String path = server.uri();
    Serial.println("handleFileRead: " + path);
    if (path.endsWith("/")) {
        path += "index.html";
    }
    String contentType = getContentType(server, path);
    String pathWithGz = path + ".gz";
    if (exists(pathWithGz) || exists(path)) {
        if (exists(pathWithGz)) {
        path += ".gz";
        }
        sendFileContent(server, path);
        return true;
    }
    return false;
    }

    void sendFileContent(WebServer& server, String path) {

        File file = FILESYSTEM.open(path, "r");
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
        server.sendContent(content);
        file.close();
    }

    String getVar(const char * pref) {
        return "$" + String(pref);      
    }
};
#endif //REQUESTHANDLERSIMPL_H
