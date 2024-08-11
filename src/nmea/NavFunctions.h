#ifndef _NavFunctions_H_
#define _NavFunctions_H_

#include <Arduino.h>
#include <math.h>
#include <NMEA0183Msg.h>

#define PI_2 6.283185307179586476925286766559

double trueToMagnetic(double trueValue, double variation);
double magneticToTrue(double magneticValue, double variation);
bool isNMEA0183NaN(double value);

#endif
