
// Demo: NMEA2000 library. Sends NMEA2000 to WiFi in NMEA0183 and SeaSmart format.
//
#define ESP32_NMEA38400_RX 5
#define ESP32_NMEA38400_TX 13 //Free pin. We don't need to use, but need to map something.
#define ESP32_NMEA4800_RX 18
#define ESP32_NMEA4800_TX 12 //Free pin. We don't need to use, but need to map something.
#define ESP32_CAN_TX_PIN GPIO_NUM_2
#define ESP32_CAN_RX_PIN GPIO_NUM_15

//Defaults for Garmin GPS120 / Garmin GPS 12
#define MAX_WP_PER_ROUTE 30
#define MAX_WP_NAME_LENGTH 6

#include <NMEA2000_CAN.h>
#include <N2kMessages.h>
#include "N2kDataToNMEA0183.h"
#include "SerialToNMEA0183.h"
#include "N2KPreferences.h"
#include "WifiConnection.h"
#include <BluetoothSerial.h>
#include <WebServer.h>
#include "PreferenceRequestHandler.h"
#include <WebSocketsServer.h>
#include "toN2K/NMEA0183Gateway.h"
#include "demo/demo.h"
#include "Hardware.h"

N2KPreferences prefs;
WifiConnection connection(&prefs);
WebServer webserver(80);
WebSocketsServer webSocket = WebSocketsServer(8080);
SerialToNMEA0183 *aisReceiver;
SerialToNMEA0183 *nmea0183Receiver;
tN2kDataToNMEA0183 *nk2To0183;
BluetoothSerial SerialBT;
Hardware hardware;
Logger* logger;
NMEA0183Gateway * pNmea0183Gateway;

std::function<void(char *)> messageCallback = [](char *message) {
  if (prefs.isNmeaToUDP()) {
    connection.sendUdpPackage(message);
  }
  if (prefs.isNmeaToBluetooth()) {
    SerialBT.println(message);
  }
  if (prefs.isNmeaToSocket()) {
    webSocket.broadcastTXT("N:" + String(message));
  }
  if (prefs.isNmeaToSerial()) {
    Serial.println(message);
  }
};

//*****************************************************************************
void InitNMEA2000()
{

  // List here messages your device will transmit.
  NMEA2000.SetN2kCANMsgBufSize(8);
  NMEA2000.SetN2kCANReceiveFrameBufSize(100);
  NMEA2000.SetForwardStream(&Serial);            // PC output on due native port
  NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text
  NMEA2000.EnableForward(prefs.isNmeaToSerial());
  tNMEA2000::tN2kMode mode = prefs.getNmeaMode();
  NMEA2000.SetMode(mode);
  if (mode == tNMEA2000::N2km_ListenAndNode || mode == tNMEA2000::N2km_NodeOnly || mode == tNMEA2000::N2km_SendOnly) {
    // List here messages your device will transmit.
    unsigned const static long TransmitMessages[] PROGMEM={127250L,129283L,129284L,129285L,126992L,129025L,129026L,129029L,0};
    NMEA2000.ExtendTransmitMessages(TransmitMessages);
    NMEA2000.SetProductInformation("00000008",107,"NMEA0183 -> N2k -> PC","1.0.0.0 (2017-07-16)","1.0.0.0 (2017-07-16)" );
    NMEA2000.SetDeviceInformation(8,130,25,2046);
  }
  NMEA2000.AttachMsgHandler(nk2To0183); // NMEA 2000 -> NMEA 0183 conversion
  NMEA2000.Open();
  Serial.println("NMEA200 initialized.");
}

//*****************************************************************************
void setup()
{
  prefs.begin();
  hardware.begin();
  hardware.handleVCCUpdate([](String message){ webSocket.broadcastTXT(message);});
  Serial.begin(115200);
  Serial1.begin(38400, SERIAL_8N1, ESP32_NMEA38400_RX, ESP32_NMEA38400_TX);
  Serial2.begin(4800, SERIAL_8N1, ESP32_NMEA4800_RX, ESP32_NMEA4800_TX);
  logger = new Logger(&Serial,DEBUG_LEVEL_TRACE);
  pNmea0183Gateway =  new NMEA0183Gateway(&NMEA2000, &Serial2, logger,MAX_WP_PER_ROUTE,MAX_WP_NAME_LENGTH);

  if (prefs.isBlEnabled())
  {
    Serial.println("Initializing bluetooth.");
    SerialBT.begin("N2K-bridge");
  }

  connection.begin();
  SPIFFS.begin();
  webserver.addHandler(new PreferenceRequestHandler(&prefs));
  webserver.begin();
  webSocket.begin();
  nk2To0183 = new tN2kDataToNMEA0183(&NMEA2000, messageCallback);
  aisReceiver = new SerialToNMEA0183(&Serial1, messageCallback);
  nmea0183Receiver = new SerialToNMEA0183(&Serial2, messageCallback);
  InitNMEA2000();
}

//*****************************************************************************
void loop()
{
  webserver.handleClient();
  webSocket.loop();

  ArduinoOTA.handle();
  if (prefs.isDemoEnabled()) {
    SendN2KMessages(nk2To0183);
  }
  NMEA2000.ParseMessages();
  nk2To0183->Update();
  aisReceiver->loop();
  nmea0183Receiver->loop();
  hardware.loop();
  tNMEA2000::tN2kMode mode = prefs.getNmeaMode();
  if (mode == tNMEA2000::N2km_ListenAndNode || mode == tNMEA2000::N2km_NodeOnly || mode == tNMEA2000::N2km_SendOnly) {
    pNmea0183Gateway->handleLoop();
  }
}