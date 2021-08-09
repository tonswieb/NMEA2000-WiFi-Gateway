/*
N2kDataToNMEA0183.cpp

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

#include "N2kToN183.h"
#include <N2kMessages.h>
#include <NMEA0183Messages.h>
#include <math.h>
#include <sstream>

const double radToDeg = 180.0 / M_PI;

template <typename T> std::string to_string(T value) {
  //create an output string stream
  std::ostringstream os;
  //throw the value into the string stream
  os << value;
  //convert the string stream into a string and return
  return os.str();
}

//*****************************************************************************
void N2kToN183::HandleMsg(const tN2kMsg &N2kMsg)
{
  if (!prefs->isNmeaSrcN2KEnabled()) {
    return;
  }

  std::string pgn = to_string(N2kMsg.PGN);
  std::string filter(prefs->getNmea200ReceiveFilter());
  if (filter.find(pgn) != std::string::npos) {
    trace("Skip PGN: %u",N2kMsg.PGN);
    return;
  }

  switch (N2kMsg.PGN)
  {
  case 126992UL:
    HandleSystemTime(N2kMsg);
  case 127250UL:
    HandleHeading(N2kMsg);
  case 127251UL:
    HandleRateOfTurn(N2kMsg);
  case 127258UL:
    HandleVariation(N2kMsg);
  case 128259UL:
    HandleBoatSpeed(N2kMsg);
  case 128267UL:
    HandleDepth(N2kMsg);
  case 129025UL:
    HandlePosition(N2kMsg);
  case 129026UL:
    HandleCOGSOG(N2kMsg);
  case 129029UL:
    HandleGNSS(N2kMsg);
  case 130306UL:
    HandleWind(N2kMsg);
  }
}

//*****************************************************************************
void N2kToN183::Update()
{
  if (!prefs->isNmeaSrcN2KEnabled()) {
    return;
  }
  SendRMC();
  if (LastHeadingTime + 2000 < millis())
    Heading = N2kDoubleNA;
  if (LastCOGSOGTime + 2000 < millis())
  {
    COG = N2kDoubleNA;
    SOG = N2kDoubleNA;
  }
  if (LastPositionTime + 4000 < millis())
  {
    gps->setLatLong(N2kDoubleNA,N2kDoubleNA);
  }
  if (LastWindTime + 2000 < millis())
  {
    WindSpeed = N2kDoubleNA;
    WindAngle = N2kDoubleNA;
  }
}

//*****************************************************************************
void N2kToN183::SendMessage(const tNMEA0183Msg &NMEA0183Msg)
{
  if (_handler_func) {
    char buf[MAX_NMEA0183_MESSAGE_SIZE];
    if (NMEA0183Msg.GetMessage(buf, MAX_NMEA0183_MESSAGE_SIZE)) {
      _handler_func (buf);
    }
  }
}

//*****************************************************************************
void N2kToN183::HandleSystemTime(const tN2kMsg &N2kMsg)
{
  unsigned char SID;
  tN2kTimeSource TimeSource;
  tNMEA0183Msg NMEA0183Msg;
  uint16_t DaysSince1970;
  double SecondsSinceMidnight;

  if (ParseN2kSystemTime(N2kMsg, SID, DaysSince1970, SecondsSinceMidnight, TimeSource))
  {
    gps->setDaysSince1970(DaysSince1970);
    gps->setSecondsSinceMidnight(SecondsSinceMidnight);
    LastSystemTime = millis();
    //TODO: Use NMEA0183 message to send date/time
  }
}

//*****************************************************************************
void N2kToN183::HandleRateOfTurn(const tN2kMsg &N2kMsg)
{
  unsigned char SID;
  double RateOfTurn;
  tNMEA0183Msg NMEA0183Msg;

  if (ParseN2kRateOfTurn(N2kMsg, SID, RateOfTurn))
  {
    tNMEA0183Msg NMEA0183Msg;
    if (NMEA0183SetROT(NMEA0183Msg, RateOfTurn))
    {
      SendMessage(NMEA0183Msg);
    }
  }
}

//*****************************************************************************
void N2kToN183::HandleHeading(const tN2kMsg &N2kMsg)
{
  unsigned char SID;
  tN2kHeadingReference ref;
  double _Deviation = 0;
  double Variation;
  tNMEA0183Msg NMEA0183Msg;

  if (ParseN2kHeading(N2kMsg, SID, Heading, _Deviation, Variation, ref))
  {
    if (ref == N2khr_magnetic)
    {
      if (!N2kIsNA(Variation))
        gps->setVariation(Variation); // Update Variation
      if (!N2kIsNA(Heading) && !N2kIsNA(Variation))
        Heading -= Variation;
    }
    LastHeadingTime = millis();
    if (NMEA0183SetHDG(NMEA0183Msg, Heading, _Deviation, Variation))
    {
      SendMessage(NMEA0183Msg);
    }
  }
}

//*****************************************************************************
void N2kToN183::HandleVariation(const tN2kMsg &N2kMsg)
{
  unsigned char SID;
  tN2kMagneticVariation Source;
  uint16_t DaysSince1970;
  double Variation;

  if (ParseN2kMagneticVariation(N2kMsg, SID, Source, DaysSince1970, Variation)) {
    gps->setDaysSince1970(DaysSince1970);
    gps->setVariation(Variation);
  }
}

//*****************************************************************************
void N2kToN183::HandleBoatSpeed(const tN2kMsg &N2kMsg)
{
  unsigned char SID;
  double WaterReferenced;
  double GroundReferenced;
  tN2kSpeedWaterReferenceType SWRT;

  if (ParseN2kBoatSpeed(N2kMsg, SID, WaterReferenced, GroundReferenced, SWRT))
  {
    tNMEA0183Msg NMEA0183Msg;
    double Variation = gps->getVariation();
    double MagneticHeading = (!N2kIsNA(Heading) && !N2kIsNA(Variation) ? Heading + Variation : NMEA0183DoubleNA);
    if (NMEA0183SetVHW(NMEA0183Msg, Heading, MagneticHeading, WaterReferenced))
    {
      SendMessage(NMEA0183Msg);
    }
  }
}

//*****************************************************************************
void N2kToN183::HandleDepth(const tN2kMsg &N2kMsg)
{
  unsigned char SID;
  double DepthBelowTransducer;
  double Offset;
  double Range;

  if (ParseN2kWaterDepth(N2kMsg, SID, DepthBelowTransducer, Offset, Range))
  {
    tNMEA0183Msg NMEA0183Msg;
    if (NMEA0183SetDPT(NMEA0183Msg, DepthBelowTransducer, Offset))
    {
      SendMessage(NMEA0183Msg);
    }
    if (NMEA0183SetDBx(NMEA0183Msg, DepthBelowTransducer, Offset))
    {
      SendMessage(NMEA0183Msg);
    }
  }
}

//*****************************************************************************
void N2kToN183::HandlePosition(const tN2kMsg &N2kMsg)
{

  double latitude; 
  double longitude;
  if (ParseN2kPGN129025(N2kMsg, latitude, longitude))
  {
    gps->setLatLong(latitude,longitude);
    LastPositionTime = millis();
    tNMEA0183Msg NMEA0183Msg;
    if (NMEA0183SetGLL(NMEA0183Msg, gps->getSecondsSinceMidnight(), latitude, longitude))
    {
      SendMessage(NMEA0183Msg);
    }
  }
}

//*****************************************************************************
void N2kToN183::HandleCOGSOG(const tN2kMsg &N2kMsg)
{
  unsigned char SID;
  tN2kHeadingReference HeadingReference;
  tNMEA0183Msg NMEA0183Msg;

  if (ParseN2kCOGSOGRapid(N2kMsg, SID, HeadingReference, COG, SOG))
  {
    LastCOGSOGTime = millis();
    double Variation = gps->getVariation();
    double MCOG = (!N2kIsNA(COG) && !N2kIsNA(Variation) ? COG - Variation : NMEA0183DoubleNA);
    if (HeadingReference == N2khr_magnetic)
    {
      MCOG = COG;
      if (!N2kIsNA(Variation))
        COG -= Variation;
    }
    if (NMEA0183SetVTG(NMEA0183Msg, COG, MCOG, SOG))
    {
      SendMessage(NMEA0183Msg);
    }
  }
}

//*****************************************************************************
void N2kToN183::HandleGNSS(const tN2kMsg &N2kMsg)
{
  unsigned char SID;
  tN2kGNSStype GNSStype;
  tN2kGNSSmethod GNSSmethod;
  unsigned char nSatellites;
  double HDOP;
  double PDOP;
  double GeoidalSeparation;
  unsigned char nReferenceStations;
  tN2kGNSStype ReferenceStationType;
  uint16_t ReferenceSationID;
  double AgeOfCorrection;
  double Latitude;
  double Longitude;
  uint16_t DaysSince1970;
  double SecondsSinceMidnight;

  if (ParseN2kGNSS(N2kMsg, SID, DaysSince1970, SecondsSinceMidnight, Latitude, Longitude, Altitude, GNSStype, GNSSmethod,
                   nSatellites, HDOP, PDOP, GeoidalSeparation,
                   nReferenceStations, ReferenceStationType, ReferenceSationID, AgeOfCorrection))
  {
    gps->setLatLong(Latitude,Longitude);
    gps->setDaysSince1970(DaysSince1970);
    gps->setSecondsSinceMidnight(SecondsSinceMidnight);
    LastPositionTime = millis();
    tNMEA0183Msg NMEA0183Msg;
    if (NMEA0183SetGGA(NMEA0183Msg, SecondsSinceMidnight, Latitude, Longitude, 1, nSatellites, HDOP, Altitude, GeoidalSeparation, AgeOfCorrection, ReferenceSationID))
    {
      SendMessage(NMEA0183Msg);
    }
  }
}

//*****************************************************************************
void N2kToN183::HandleWind(const tN2kMsg &N2kMsg)
{
  unsigned char SID;
  tN2kWindReference WindReference;
  tNMEA0183WindReference NMEA0183Reference = NMEA0183Wind_True;

  if (ParseN2kWindSpeed(N2kMsg, SID, WindSpeed, WindAngle, WindReference))
  {
    tNMEA0183Msg NMEA0183Msg;
    LastWindTime = millis();
    if (WindReference == N2kWind_Apparent)
      NMEA0183Reference = NMEA0183Wind_Apparent;

    if (NMEA0183SetMWV(NMEA0183Msg, WindAngle * radToDeg, NMEA0183Reference, WindSpeed))
    {
      SendMessage(NMEA0183Msg);
    }
  }
}

//*****************************************************************************
void N2kToN183::SendRMC()
{
  if (NextRMCSend <= millis() && !N2kIsNA(gps->getLatitude()))
  {
    tNMEA0183Msg NMEA0183Msg;
    if (NMEA0183SetRMC(NMEA0183Msg, gps->getSecondsSinceMidnight(), gps->getLatitude(), gps->getLongitude(), COG, SOG, gps->getDaysSince1970(), gps->getVariation()))
    {
      SendMessage(NMEA0183Msg);
    }
    SetNextRMCSend();
  }
}
