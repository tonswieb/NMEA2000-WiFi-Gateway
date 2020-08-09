#ifndef _INIT_N2K_H_
#define _INIT_N2K_H_

//CAN PIN's must be defined before NMEA2000_CAN.h is included!
#define ESP32_CAN_TX_PIN GPIO_NUM_2
#define ESP32_CAN_RX_PIN GPIO_NUM_15

#include <NMEA2000_CAN.h>
#include "prefs/N2KPreferences.h"
#include "N2kToN183.h"

String printMode(tNMEA2000::tN2kMode _N2kMode) {

  switch (_N2kMode) {

    case tNMEA2000::N2km_ListenOnly:
      return "NMEA200 mode: Listen Only.";
    case tNMEA2000::N2km_NodeOnly:
      return "NMEA200 mode: Node Only.";
    case tNMEA2000::N2km_ListenAndNode:
      return "NMEA200 mode: Listen and Node.";
    case tNMEA2000::N2km_SendOnly:
      return "NMEA200 mode: Send Only.";
    case tNMEA2000::N2km_ListenAndSend:
      return "NMEA200 mode: Listen and Send.";
  }
}

//*****************************************************************************
void InitNMEA2000(N2KPreferences *prefs, N2kToN183 *pN2kToN183, Stream *logger)
{

  // List here messages your device will transmit.
  NMEA2000.SetN2kCANMsgBufSize(8);
  NMEA2000.SetN2kCANReceiveFrameBufSize(100);
  NMEA2000.SetForwardStream(logger);            // PC output on due native port
  NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text
  NMEA2000.EnableForward(prefs->isNmeaToSerial());
  tNMEA2000::tN2kMode mode = prefs->getNmeaMode();
  logger->println(printMode(mode));
  NMEA2000.SetMode(mode);
  if (mode == tNMEA2000::N2km_ListenAndNode || mode == tNMEA2000::N2km_NodeOnly || mode == tNMEA2000::N2km_SendOnly) {
    logger->println("Initializing Product and Device Information");
    // List here messages your device will transmit.
    unsigned const static long TransmitMessages[] PROGMEM={127250L,129283L,129284L,129285L,126992L,129025L,129026L,129029L,0};
    NMEA2000.ExtendTransmitMessages(TransmitMessages);
    NMEA2000.SetProductInformation("00000008",107,"NMEA 0183 WIFI/Bluetooth Gateway","1.0.0.0 (2020-08-09)","1.0.0.0 (2020-08-09)" );
    //Set to NMEA 0183 Gateway according to http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
    //Set IndustryGroup = 2046 because not used in: http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
    NMEA2000.SetDeviceInformation(1,135,25,2046);
  }
  NMEA2000.AttachMsgHandler(pN2kToN183); // NMEA 2000 -> NMEA 0183 conversion
  NMEA2000.Open();
  logger->println("NMEA200 initialized.");
}

#endif