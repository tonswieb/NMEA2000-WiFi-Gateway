
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
#include <N2kMessages.h>
#include "N2kDataToNMEA0183.h"
#include "SerialToNMEA0183.h"
#include "N2KPreferences.h"
#include "WifiConnection.h"
#include <FastLED.h>
#include <BluetoothSerial.h>
#include <driver/rtc_io.h> //needed for using the ESP-PICO-D4 IO pins
#include <Wire.h>
#include <WebServer.h>
#include "PreferenceRequestHandler.h"

N2KPreferences prefs;
WifiConnection connection (&prefs);
WebServer webserver (80);
SerialToNMEA0183 *aisReceiver;
SerialToNMEA0183 *nmea0183Receiver;
tN2kDataToNMEA0183 *nk2To0183;
BluetoothSerial SerialBT;

long Blink;
double VCC = 0.0;

std::function<void(char *)> messageCallback = [](char *message) {
  connection.sendUdpPackage(message);
  if (prefs.isBlEnabled())
  {
    SerialBT.println(message);
  }
  Serial.println(message);
};

//*****************************************************************************
void InitNMEA2000()
{

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
void InitHardware()
{
  pinMode(Relais, OUTPUT);
  digitalWrite(Relais, LOW);
  pinMode(MOB, INPUT);
}

//*****************************************************************************
void setup()
{
  prefs.begin();
  InitHardware();
  Serial.begin(115200);
  Serial1.begin(38400, SERIAL_8N1, ESP32_NMEA38400_RX, ESP32_NMEA38400_TX);
  Serial2.begin(4800, SERIAL_8N1, ESP32_NMEA4800_RX, ESP32_NMEA4800_TX);

  if (prefs.isBlEnabled()) {
    Serial.println("Initializing bluetooth.");
    SerialBT.begin("N2K-bridge");
  }  
  Wire.begin(26, 25);

  connection.begin();
  SPIFFS.begin();
  webserver.addHandler(new PreferenceRequestHandler(&prefs));
  webserver.begin();
  nk2To0183 = new tN2kDataToNMEA0183(&NMEA2000, messageCallback);
  aisReceiver = new SerialToNMEA0183(&Serial1, messageCallback);
  nmea0183Receiver = new SerialToNMEA0183(&Serial2, messageCallback);
  InitNMEA2000();
}

//*****************************************************************************
void loop()
{
  webserver.handleClient();
  ArduinoOTA.handle();
  SendN2KMessages();

  NMEA2000.ParseMessages();
  nk2To0183->Update();
  aisReceiver->loop();
  nmea0183Receiver->loop();
  if (Blink + 1000 < millis())
  {
    Blink = millis();
    digitalWrite(Relais, !digitalRead(Relais)); //relais test. should be toggeling with 1 Hz
    VCC = analogRead(Vin) * 3.6 * 5.7 / 4095;
  }
}

#define UpdatePeriod 5000

void SendN2KMessages()
{
  if (!prefs.isDemoEnabled())
  {
    return;
  }
  unsigned char seq = 1;
  uint16_t DaysSince1970 = 18090;
  double magHeading = 290.0;
  double variation = 10.0;
  double deviation = 0.0;
  double waterSpeed = 6.0;
  double groundSpeed = 6.0;
  double depthBelowTransducer = 2.0;
  double depthTransducerOffset = 0.0;
  double windSpeed = 15; //m/s
  double windAngle = DegToRad(90);
  double cog = 290.0;
  double sog = 6.0;
  double lat = 30.0;
  double lon = 20.0;
  double altitude = 1.0;
  double SecondsSinceMidnight = 7 * 3600;
  unsigned char nSatellites = 5;
  double HDOP = 0.0;

  static unsigned long Updated = millis();
  tN2kMsg N2kMsg;

  if (Updated + UpdatePeriod < millis())
  {
    Updated = millis();

    SetN2kTrueHeading(N2kMsg, seq, magHeading + variation);
    nk2To0183->HandleMsg(N2kMsg);
    SetN2kMagneticHeading(N2kMsg, seq, magHeading, deviation, variation);
    nk2To0183->HandleMsg(N2kMsg);
    SetN2kMagneticVariation(N2kMsg, seq, N2kmagvar_Manual, DaysSince1970, variation);
    nk2To0183->HandleMsg(N2kMsg);
    SetN2kBoatSpeed(N2kMsg, seq, waterSpeed, groundSpeed, N2kSWRT_Paddle_wheel);
    nk2To0183->HandleMsg(N2kMsg);
    SetN2kWaterDepth(N2kMsg, seq, depthBelowTransducer, depthTransducerOffset);
    nk2To0183->HandleMsg(N2kMsg);
    SetN2kLatLonRapid(N2kMsg, lat, lon);
    nk2To0183->HandleMsg(N2kMsg);
    SetN2kCOGSOGRapid(N2kMsg, seq, N2khr_magnetic, cog, sog);
    nk2To0183->HandleMsg(N2kMsg);
    SetN2kWindSpeed(N2kMsg, seq, windSpeed, windAngle, N2kWind_Apparent);
    nk2To0183->HandleMsg(N2kMsg);
    SetN2kGNSS(N2kMsg, seq, DaysSince1970, SecondsSinceMidnight, lat, lon, altitude, N2kGNSSt_GPSSBASWAASGLONASS, N2kGNSSm_PreciseGNSS, nSatellites, HDOP);
    nk2To0183->HandleMsg(N2kMsg);
  }
}
