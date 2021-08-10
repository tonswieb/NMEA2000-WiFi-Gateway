#ifndef _N2K_TO_N183_H_
#define _N2K_TO_N183_H_

/*
N2kDataToNMEA0183.h

Copyright (c) 2015-2018 Timo Lappalainen, Kave Oy, www.kave.fi

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <NMEA0183.h>
#include <NMEA2000.h>
#include <functional>
#include <util/Log.h>
#include <prefs/N2KPreferences.h>
#include "nmea/Gps.h"

#define MAX_NMEA0183_MESSAGE_SIZE 100

//------------------------------------------------------------------------------
class N2kToN183 : public tNMEA2000::tMsgHandler {

private:
  Logger* logger;

public:
  using tSendNMEA0183MessageCallback=void (*)(const tNMEA0183Msg &NMEA0183Msg);
    
protected:
  static const unsigned long RMCPeriod=1000;
  double Altitude;
  double Heading;
  double WindSpeed;
  double WindAngle;
  unsigned long LastHeadingTime;
  unsigned long LastCOGSOGTime;
  unsigned long LastPositionTime;
  unsigned long LastPosSend;
  unsigned long LastWindTime;
  unsigned long LastSystemTime;
  unsigned long NextRMCSend;
  Gps *gps;

  std::function<void (char*)> _handler_func;
  N2KPreferences *prefs;

protected:
  void HandleSystemTime(const tN2kMsg &N2kMsg); // 126992
  void HandleHeading(const tN2kMsg &N2kMsg); // 127250
  void HandleRateOfTurn(const tN2kMsg &N2kMsg); // 127251
  void HandleVariation(const tN2kMsg &N2kMsg); // 127258
  void HandleBoatSpeed(const tN2kMsg &N2kMsg); // 128259
  void HandleDepth(const tN2kMsg &N2kMsg); // 128267
  void HandlePosition(const tN2kMsg &N2kMsg); // 129025
  void HandleCOGSOG(const tN2kMsg &N2kMsg); // 129026
  void HandleGNSS(const tN2kMsg &N2kMsg); // 129029
  void HandleWind(const tN2kMsg &N2kMsg); // 130306
  void SetNextRMCSend() { NextRMCSend=millis()+RMCPeriod; }
  void SendRMC();
  void SendMessage(const tNMEA0183Msg &NMEA0183Msg);

public:
  N2kToN183(tNMEA2000 *_pNMEA2000, Gps *gps, std::function<void (char*)> handler_func, N2KPreferences *prefs, Logger* logger) : tNMEA2000::tMsgHandler(0,_pNMEA2000) {
    _handler_func = handler_func;
    Altitude=N2kDoubleNA;
    //TODO: Move initialization and reset to Gps class without introducing dependency on NMEA2000. Requires refactoring N2kIsNaN.
    gps->setLatLong(N2kDoubleNA,N2kDoubleNA);
    gps->setVariation(N2kDoubleNA);
    gps->setDaysSince1970(N2kUInt16NA);
    gps->setCOG(N2kDoubleNA);
    gps->setSOG(N2kDoubleNA);
    Heading=N2kDoubleNA;
    LastPosSend=0;
    NextRMCSend=millis()+RMCPeriod;
    LastHeadingTime=0;
    LastCOGSOGTime=0;
    LastPositionTime=0;
    LastWindTime=0;
    LastSystemTime=0;
    this->prefs = prefs;
    this->logger = logger;
    this->gps = gps;
  }
  void HandleMsg(const tN2kMsg &N2kMsg);
  void Update();
};

#endif
