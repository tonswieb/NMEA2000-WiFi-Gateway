// Demo: NMEA2000 library. Sends NMEA2000 to WiFi in NMEA0183 and SeaSmart format.
//
// The code has been tested with ESP32.

#define ESP32_NMEA38400_RX 5
#define ESP32_CAN_TX_PIN GPIO_NUM_12
#define ESP32_CAN_RX_PIN GPIO_NUM_13
#include <NMEA2000_CAN.h>
#include <WiFi.h>
#include <nvs.h>
#include <nvs_flash.h>
#include "N2kDataToNMEA0183.h"
#include <WiFiUdp.h>
#include "SerialToNMEA0183.h"

// Define your default settings here
const char *ssid = "ARISTO"; //Replace with WIFI name.
const char *udpAddress = "192.168.4.255";
const int port = 9876;
WiFiUDP udp;
SerialToNMEA0183* aisReceiver;

bool ResetWiFiSettings=true; // If you have tested other code in your module, it may have saved settings and have difficulties to make connection.

tN2kDataToNMEA0183 tN2kDataToNMEA0183(&NMEA2000, 0);

// Forward declarations
void SendNMEA0183Message(const tNMEA0183Msg &NMEA0183Msg);
void InitNMEA2000();

#include <nvs.h>
#include <nvs_flash.h>

void ResetWiFiSettingsOnNvs() {
  int err;
  err=nvs_flash_init();
  Serial.println("nvs_flash_init: " + err);
  err=nvs_flash_erase();
  Serial.println("nvs_flash_erase: " + err); 
}

//*****************************************************************************
void setup() {
  if ( ResetWiFiSettings ) ResetWiFiSettingsOnNvs();

  Serial.begin(115200);
  Serial1.begin(38400, SERIAL_8N1, ESP32_NMEA38400_RX);

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  delay(1000);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);
  
  InitNMEA2000();
  Serial.println("NMEA200 initialized.");

  aisReceiver = new SerialToNMEA0183(&Serial1);
}

//*****************************************************************************
void loop() {
  NMEA2000.ParseMessages();
  tN2kDataToNMEA0183.Update();
  SendNMEA0183Message(aisReceiver);
}

//*****************************************************************************
void InitNMEA2000() {
  NMEA2000.SetN2kCANMsgBufSize(8);
  NMEA2000.SetN2kCANReceiveFrameBufSize(100);
  NMEA2000.SetForwardStream(&Serial);  // PC output on due native port
  NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text                               
  NMEA2000.SetMode(tNMEA2000::N2km_ListenOnly);
  NMEA2000.AttachMsgHandler(&tN2kDataToNMEA0183); // NMEA 2000 -> NMEA 0183 conversion
  
  tN2kDataToNMEA0183.SetSendNMEA0183MessageCallback(SendNMEA0183Message);

  NMEA2000.Open();
}

#define MAX_NMEA0183_MESSAGE_SIZE 100
//*****************************************************************************
void SendNMEA0183Message(const tNMEA0183Msg &NMEA0183Msg) {
  
  char buf[MAX_NMEA0183_MESSAGE_SIZE];
  if ( !NMEA0183Msg.GetMessage(buf,MAX_NMEA0183_MESSAGE_SIZE) ) return;
  udp.beginPacket(udpAddress, port);
  udp.println(buf);
  udp.endPacket();
}

void SendNMEA0183Message(SerialToNMEA0183* serialToNMEA0183) {
  while (serialToNMEA0183->parseMessage()) {
    udp.beginPacket(udpAddress, port);
    udp.println(serialToNMEA0183->getMessage());
    udp.endPacket();
  }
}
