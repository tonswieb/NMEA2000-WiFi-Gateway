
// Demo: NMEA2000 library. Sends NMEA2000 to WiFi in NMEA0183 and SeaSmart format.
//
// The code has been tested with ESP32.
#define NUM_LEDS 2   //number of WS8212B leds
#define WS2812_PIN 4 //ouput to pin WS2812
#define Vin 37       //Board voltage measring input
#define MOB 38       //man over board input
#define Relais 0     //ouput extarnal power (12V 500mA max)

#define ESP32_NMEA38400_RX 5
#define ESP32_NMEA38400_TX 13 //Free pin. We don't need to use, but need to map something.
#define ESP32_NMEA4800_RX 18
#define ESP32_NMEA4800_TX 12 //Free pin. We don't need to use, but need to map something.
#define ESP32_CAN_TX_PIN GPIO_NUM_2
#define ESP32_CAN_RX_PIN GPIO_NUM_15

#include <NMEA2000_CAN.h>
#include "N2kDataToNMEA0183.h"
#include "SerialToNMEA0183.h"
#include "WifiConnection.h"
#include <FastLED.h>
#include <driver/rtc_io.h> //needed for using the ESP-PICO-D4 IO pins

WifiConnection *connection;
SerialToNMEA0183 *aisReceiver;
SerialToNMEA0183 *nmea0183Receiver;
tN2kDataToNMEA0183 *nk2To0183; 

std::function<void (char*)> sendViaUdp = [] (char* message) {connection->sendUdpPackage(message);};

//*****************************************************************************
void InitNMEA2000() {

  NMEA2000.SetN2kCANMsgBufSize(8);
  NMEA2000.SetN2kCANReceiveFrameBufSize(100);
  NMEA2000.SetForwardStream(&Serial);            // PC output on due native port
  NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text
  NMEA2000.SetMode(tNMEA2000::N2km_ListenOnly);
  NMEA2000.AttachMsgHandler(nk2To0183); // NMEA 2000 -> NMEA 0183 conversion
  NMEA2000.Open();
  Serial.println("NMEA200 initialized.");
}

//*****************************************************************************
void setup()
{
  Serial.begin(115200);
  Serial1.begin(38400, SERIAL_8N1, ESP32_NMEA38400_RX, ESP32_NMEA38400_TX);
  Serial2.begin(4800, SERIAL_8N1, ESP32_NMEA4800_RX, ESP32_NMEA4800_TX);
 
  connection = new WifiConnection(); 
  nk2To0183 = new tN2kDataToNMEA0183(&NMEA2000, sendViaUdp);
  aisReceiver = new SerialToNMEA0183(&Serial1, sendViaUdp);
  nmea0183Receiver = new SerialToNMEA0183(&Serial2, sendViaUdp);
  InitNMEA2000();
}

//*****************************************************************************
void loop() {

  ArduinoOTA.handle();
  NMEA2000.ParseMessages();
  nk2To0183->Update();
  aisReceiver->loop();
  nmea0183Receiver->loop();
}