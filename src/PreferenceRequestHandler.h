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
        return true;
    }

    bool handle(WebServer &server, HTTPMethod requestMethod, String requestUri) override
    {
        if (server.hasArg("handleForm"))
        {
            Serial.println("Handle Form");
            _prefs->setBlEnabled(server.hasArg("blEnabled"));
            _prefs->setDemoEnabled(server.hasArg("demoEnabled"));
        }
        if (!handleFileRead(server))
        {
            server.send(404, "text/plain", "FileNotFound");
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
        if (path.endsWith("/"))
        {
            path += "index.html";
        }
        if (path.endsWith("/readADC"))
        {
            server.send(200, "text/plane", String(VCC));
        }

        String contentType = getContentType(server, path);
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
        char temp[10];
        File file = FILESYSTEM.open(path, "r");
        String content = file.readString();
        content.replace("$blEnabled", _prefs->isBlEnabled() ? "checked" : "");
        content.replace("$demoEnabled", _prefs->isDemoEnabled() ? "checked" : "");
        server.sendContent(content);
        file.close();
    }
};

#endif //REQUESTHANDLERSIMPL_H
