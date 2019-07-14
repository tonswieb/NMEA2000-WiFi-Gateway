// Demo: NMEA2000 library. Sends NMEA2000 to WiFi in NMEA0183 and SeaSmart format.
//
// The code has been tested with ESP32.

#define ESP32_CAN_TX_PIN GPIO_NUM_12
#define ESP32_CAN_RX_PIN GPIO_NUM_13
#include <NMEA2000_CAN.h>
#include <WiFi.h>
#include <nvs.h>
#include <nvs_flash.h>
#include "N2kDataToNMEA0183.h"
#include <WiFiUdp.h>

// Define your default settings here
const char *ssid = "ARISTO"; //Replace with WIFI name.
const char *udpAddress = "192.168.4.255";
const int port = 9876;
WiFiUDP udp;

bool ResetWiFiSettings=true; // If you have tested other code in your module, it may have saved settings and have difficulties to make connection.

tN2kDataToNMEA0183 tN2kDataToNMEA0183(&NMEA2000, 0);

// Set the information for other bus devices, which messages we support
const unsigned long TransmitMessages[] PROGMEM={0};
const unsigned long ReceiveMessages[] PROGMEM={/*126992L,*/ // System time
                                              60928L,
                                              126992UL, //System time
                                              126993L,
                                              127250L, // Heading
                                              127258L, // Magnetic variation
                                              128259UL,// Boat speed
                                              128267UL,// Depth
                                              129025UL,// Position
                                              129026L, // COG and SOG
                                              129029L, // GNSS
                                              130306L, // Wind
                                              0};

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

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  delay(1000);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);
  
  InitNMEA2000();
  Serial.println("NMEA200 initialized.");
}

//*****************************************************************************
void loop() {
  NMEA2000.ParseMessages();
  tN2kDataToNMEA0183.Update();
  // Dummy to empty input buffer to avoid board to stuck with e.g. NMEA Reader
  if ( Serial.available() ) { Serial.read(); } 
}

//*****************************************************************************
void InitNMEA2000() {
  NMEA2000.SetN2kCANMsgBufSize(8);
  NMEA2000.SetN2kCANReceiveFrameBufSize(100);
  NMEA2000.SetForwardStream(&Serial);  // PC output on due native port
  NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text
  NMEA2000.SetProductInformation("00000002", // Manufacturer's Model serial code
                                 130, // Manufacturer's product code
                                 "N2k->NMEA0183 WiFi",  // Manufacturer's Model ID
                                 "1.0.0.1 (2018-04-08)",  // Manufacturer's Software version code
                                 "1.0.0.0 (2018-04-08)" // Manufacturer's Model version
                                 );
  // Det device information
  NMEA2000.SetDeviceInformation(112234, // Unique number. Use e.g. Serial number.
                                130, // Device function=PC Gateway. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20%26%20function%20codes%20v%202.00.pdf
                                25, // Device class=Inter/Intranetwork Device. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20%26%20function%20codes%20v%202.00.pdf
                                2046 // Just choosen free from code list on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
                               );
                               
  NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode,32);
  NMEA2000.ExtendTransmitMessages(TransmitMessages);
  NMEA2000.ExtendReceiveMessages(ReceiveMessages);
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
