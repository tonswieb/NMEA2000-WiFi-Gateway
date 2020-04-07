#include <WebServer.h>
#include <SPIFFS.h>
#define FILESYSTEM SPIFFS

class HtmlServer {

private:
  WebServer* server;
  String getContentType(String filename);
  bool exists(String path);
  bool handleFileRead(String path);

public:
  HtmlServer();
  void loop();
};