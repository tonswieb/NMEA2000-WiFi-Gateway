#define ESP32_NMEA38400_RX 5
#define ESP32_NMEA38400_TX 13 //Free pin. We don't need to use, but need to map something.
#define ESP32_NMEA4800_RX 18
#define ESP32_NMEA4800_TX 12 //Free pin. We don't need to use, but need to map something.

#include <SPIFFS.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

#include "nmea/InitN2k.h"
#include "nmea/N2kToN183.h"
#include "nmea/StreamToN183.h"
#include "nmea/N183ToN2k.h"
#include "nmea/demo/demo.h"
#include "prefs/N2KPreferences.h"
#include "prefs/PreferenceRequestHandler.h"
#include "wifi/WifiConnection.h"
#include "wifi/BluetoothGps.h"
#include "util/Hardware.h"

N2KPreferences prefs;
WifiConnection connection(&prefs);
WebServer webserver(80);
WebSocketsServer webSocket = WebSocketsServer(8080);
BluetoothGps bluetooth(&prefs);
Hardware hardware;
Logger* logger;

StreamToN183 *pSerial1ToN2k;
N183ToN2k * pSerial2ToN2k;
N183ToN2k * pUdpToN2k;
N2kToN183 * pN2kToN183;

std::function<void(char *)> messageCallback = [](char *message) {

  connection.sendUdpPackage(message);
  bluetooth.sendUdpPackage(message);

  if (prefs.isNmeaToSocket()) {
    webSocket.broadcastTXT("N:" + String(message));
  }
  if (prefs.isNmeaToSerial()) {
    Serial.println(message);
  }
};

//*****************************************************************************
void setup()
{
  prefs.begin();
  hardware.begin();
  hardware.handleVCCUpdate([](String message){ webSocket.broadcastTXT(message);});
  SPIFFS.begin();
  connection.begin();
  webserver.addHandler(new PreferenceRequestHandler(&prefs));
  webserver.begin();
  webSocket.begin();
  Serial.begin(115200);
  Serial1.begin(38400, SERIAL_8N1, ESP32_NMEA38400_RX, ESP32_NMEA38400_TX);
  Serial2.begin(4800, SERIAL_8N1, ESP32_NMEA4800_RX, ESP32_NMEA4800_TX);
  bluetooth.begin();

  logger = new Logger(&Serial,DEBUG_LEVEL_TRACE);
  pSerial1ToN2k = new StreamToN183(&Serial1, messageCallback);
  pSerial2ToN2k =  new N183ToN2k(&NMEA2000, &Serial2, logger,MAX_WP_PER_ROUTE,MAX_WP_NAME_LENGTH);
  pUdpToN2k =  new N183ToN2k(&NMEA2000, connection.getUdpPackageStream(), logger,MAX_WP_PER_ROUTE,MAX_WP_NAME_LENGTH);
  pN2kToN183 = new N2kToN183(&NMEA2000, messageCallback);
  InitNMEA2000(&prefs,pN2kToN183);
  prefs.freeEntries();
}

//*****************************************************************************
void loop()
{
  webserver.handleClient();
  webSocket.loop();
  NMEA2000.ParseMessages();
  pN2kToN183->Update();
  pSerial1ToN2k->loop();
  hardware.loop();
  pSerial2ToN2k->handleLoop();
  pUdpToN2k->handleLoop();
  connection.loop();
  bluetooth.loop();

  if (prefs.isDemoEnabled()) {
    SendN2KMessages(pN2kToN183);
  }
}