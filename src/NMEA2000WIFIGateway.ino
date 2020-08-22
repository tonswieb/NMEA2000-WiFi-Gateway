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
#include "wifi/SuspendableHardwareSerial.h"
#include "wifi/WifiConnection.h"
#include "wifi/Bluetooth.h"
#include "wifi/WebSocketStream.h"
#include "wifi/MulticastStream.h"
#include "util/Hardware.h"

HardwareSerial Serial(0);
WebServer webserver(80);
WebSocketsServer webSocketServer = WebSocketsServer(8080);
WebSocketStream webLog = WebSocketStream(&webSocketServer);
MulticastStream multiLog = MulticastStream(&Serial,&webLog);
Logger logger = Logger(&multiLog);
N2KPreferences prefs(&logger);
WifiConnection wifiClient(&prefs,&logger);
BluetoothSerial SerialBT;
Bluetooth bluetooth(&SerialBT,&prefs,&logger);
SuspendableHardwareSerial Serial1(1,&logger);
SuspendableHardwareSerial Serial2(2,&logger);
Hardware hardware;

StreamToN183 *pSerial1ToN183;
StreamToN183 *pSerialBtToN183;
N183ToN2k * pSerial2ToN2k;
N183ToN2k * pUdpToN2k;
N2kToN183 * pN2kToN183;

std::function<void(char *)> nmea0183MessageHandler = [](char *message) {

  wifiClient.sendUdpPackage(message);
  bluetooth.sendUdpPackage(message);

  if (prefs.isNmeaToSocket()) {
    webSocketServer.broadcastTXT("N:" + String(message));
  }
  if (prefs.isNmeaToSerial()) {
    multiLog.println(message);
  }
};

//*****************************************************************************
void setup()
{
  Serial.begin(115200);
  Serial1.begin(38400, SERIAL_8N1, ESP32_NMEA38400_RX, ESP32_NMEA38400_TX);
  Serial2.begin(4800, SERIAL_8N1, ESP32_NMEA4800_RX, ESP32_NMEA4800_TX);
  
  prefs.setNmeaSrcSerial1Callback([](bool enable) {Serial1.suspend(!enable);});
  prefs.setNmeaSrcSerial2Callback([](bool enable) {Serial2.suspend(!enable);});
  prefs.setLogLevelCallBack([](int level) {logger.setLevel(level);});
  //Initialize callbacks before begin() so preferences are correctly initialized on all callbacks.
  prefs.begin();
  hardware.begin();
  hardware.handleVCCUpdate([](String message){ webSocketServer.broadcastTXT(message);});
  SPIFFS.begin();
  wifiClient.begin();
  webserver.addHandler(new PreferenceRequestHandler(&prefs,&logger));
  webserver.begin();
  webSocketServer.begin();
  bluetooth.begin();
  pSerial1ToN183 = new StreamToN183(&Serial1, nmea0183MessageHandler);
  pSerialBtToN183 = new StreamToN183(&SerialBT, nmea0183MessageHandler);
  //TODO: Add multicast to send Serial2 to NNMEA-0183 receivers as well and not only to N2K
  pSerial2ToN2k =  new N183ToN2k(&NMEA2000, &Serial2, &logger, &prefs, MAX_WP_PER_ROUTE,MAX_WP_NAME_LENGTH);
  pUdpToN2k =  new N183ToN2k(&NMEA2000, wifiClient.getUdpPackageStream(), &logger, &prefs, MAX_WP_PER_ROUTE,MAX_WP_NAME_LENGTH);
  pN2kToN183 = new N2kToN183(&NMEA2000, nmea0183MessageHandler,&prefs, &logger);
  InitNMEA2000(&prefs,pN2kToN183, &logger);
  prefs.setNmea2000Callback([]() {InitNMEA2000(&prefs,pN2kToN183, &logger);});
  prefs.freeEntries();
}

//*****************************************************************************
void loop() {

  webserver.handleClient();
  webSocketServer.loop();
  NMEA2000.ParseMessages();
  pN2kToN183->Update();
  pSerial1ToN183->loop();
  pSerialBtToN183->loop();
  hardware.loop();
  pSerial2ToN2k->handleLoop();
  pUdpToN2k->handleLoop();
  wifiClient.loop();

  if (prefs.isDemoEnabled()) {
    SendN2KMessages(pN2kToN183);
  }
}