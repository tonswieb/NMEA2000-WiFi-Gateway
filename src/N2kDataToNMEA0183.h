#ifndef _N2K_DATA_TO_NMEA0183_H_
#define _N2K_DATA_TO_NMEA0183_H_

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
#define MAX_NMEA0183_MESSAGE_SIZE 100

//------------------------------------------------------------------------------
class tN2kDataToNMEA0183 : public tNMEA2000::tMsgHandler {
public:
  using tSendNMEA0183MessageCallback=void (*)(const tNMEA0183Msg &NMEA0183Msg);
    
protected:
  static const unsigned long RMCPeriod=1000;
  double Latitude;
  double Longitude;
  double Altitude;
  double Variation;
  double Heading;
  double COG;
  double SOG;
  double WindSpeed;
  double WindAngle;
  unsigned long LastHeadingTime;
  unsigned long LastCOGSOGTime;
  unsigned long LastPositionTime;
  unsigned long LastPosSend;
  unsigned long LastWindTime;
  unsigned long LastSystemTime;
  uint16_t DaysSince1970;
  double SecondsSinceMidnight;
  unsigned long NextRMCSend;

  std::function<void (char*)> _handler_func;

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
  tN2kDataToNMEA0183(tNMEA2000 *_pNMEA2000, std::function<void (char*)> handler_func) : tNMEA2000::tMsgHandler(0,_pNMEA2000) {
    _handler_func = handler_func;
    Latitude=N2kDoubleNA; Longitude=N2kDoubleNA; Altitude=N2kDoubleNA;
    Variation=N2kDoubleNA; Heading=N2kDoubleNA; COG=N2kDoubleNA; SOG=N2kDoubleNA;
    SecondsSinceMidnight=N2kDoubleNA; DaysSince1970=N2kUInt16NA;
    LastPosSend=0;
    NextRMCSend=millis()+RMCPeriod;
    LastHeadingTime=0;
    LastCOGSOGTime=0;
    LastPositionTime=0;
    LastWindTime=0;
    LastSystemTime=0;
  }
  void HandleMsg(const tN2kMsg &N2kMsg);
  void Update();
};

#endif
