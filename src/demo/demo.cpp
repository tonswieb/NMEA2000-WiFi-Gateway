#include "demo.h"

void SendN2KMessages(tN2kDataToNMEA0183 *nk2To0183) {
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