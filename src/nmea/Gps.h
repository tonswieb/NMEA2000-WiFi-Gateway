#ifndef _Gps_H_
#define _Gps_H_

#include <Arduino.h>
#include <math.h>

/**
 * Holds the information received from a GPS sensor.
 */
class Gps {

public:
  const static int EXPERIATION_IN_MILLIS = 4000;

protected:
  double latitude = NAN; 
  double longitude = NAN;
  uint16_t daysSince1970 = 0;   // Days since 1970-01-01
  double secondsSinceMidnight = NAN;
  double variation = NAN;
  unsigned long lastPositionTime = 0;

public:
  bool isValid();
  void setLatLong(double latitude, double longitude);
  double getLatitude();
  double getLongitude();
  void setVariation(double variation);
  double getVariation();
  void setDaysSince1970(unsigned long daysSince1970);
  unsigned long getDaysSince1970();
  void setSecondsSinceMidnight(double secondsSinceMidnight);
  double getSecondsSinceMidnight();
};

#endif
