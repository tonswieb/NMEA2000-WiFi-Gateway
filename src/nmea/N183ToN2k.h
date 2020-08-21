/* 
NMEA0183GPSGateway.h

2015 Copyright (c) Kave Oy, www.kave.fi  All right reserved.

Author: Timo Lappalainen, Ton Swieb

  This library is free software; you can redistribute it and/or
  modify it as you like.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/
 
#ifndef _NMEA0183Gateway_H_
#define _NMEA0183Gateway_H_

#include <Arduino.h>
#include <N2kMsg.h>
#include <NMEA2000.h>
#include <N2kMessages.h>
#include <NMEA0183.h>
#include <NMEA0183Msg.h>
#include <NMEA0183Messages.h>
#include <Time.h>
#include "util/Log.h"
#include <prefs/N2KPreferences.h>
#include "Route.h"

class N183ToN2k {

  private:  
    unsigned long DaysSince1970;   // Days since 1970-01-01
    double Latitude, Longitude;
    double Variation = 0;
    //NMEA0183-BOD (Bearing Origin to Destination). Does not conatin enough information to send a NMEA200 message, but contains some elements required for a NMEA2000 message.
    tBOD bod;
    Route* route;
    tNMEA0183 NMEA0183;
    tNMEA2000* pNMEA2000;
    Logger* logger;
    N2KPreferences *prefs;

    void sendPGN127250(const double &heading);
    void sendPGN129025(const double &latitude, const double &longitude);
    void sendPGN129026(const tN2kHeadingReference ref, const double &COG, const double &SOG);
    void sendPGN129029(const tGGA &gga);
    void sendPGN129283(const tRMB &rmb);
    void sendPGN129284(const tRMB &rmb);
    void sendPGN129285();
    void sendPGN129285(const tRMB &rmb);

    void HandleNMEA0183Msg(const tNMEA0183Msg &NMEA0183Msg);
    void HandleRMB(const tNMEA0183Msg &NMEA0183Msg);
    void HandleRMC(const tNMEA0183Msg &NMEA0183Msg);
    void HandleGGA(const tNMEA0183Msg &NMEA0183Msg);
    void HandleHDT(const tNMEA0183Msg &NMEA0183Msg);
    void HandleVTG(const tNMEA0183Msg &NMEA0183Msg);
    void HandleBOD(const tNMEA0183Msg &NMEA0183Msg);
    void HandleRTE(const tNMEA0183Msg &NMEA0183Msg);
    void HandleWPL(const tNMEA0183Msg &NMEA0183Msg);
    void HandleGLL(const tNMEA0183Msg &NMEA0183Msg);
  
  public:
    N183ToN2k(tNMEA2000* pNMEA2000, Stream* nmea0183, Logger* logger, N2KPreferences *prefs, byte maxWpPerRoute = MAX_WP_PER_ROUTE, byte maxWpNameLength = MAX_WP_NAME_LENGTH);
    void handleLoop();
};

#endif

