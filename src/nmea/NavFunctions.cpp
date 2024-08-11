#include <nmea/NavFunctions.h>

double magneticToTrue(double magneticValue, double variation) {

  double trueValue=magneticValue+variation;
  while (trueValue<0) trueValue+=PI_2;
  while (trueValue>=PI_2) trueValue-=PI_2;
  return trueValue;    
}

double trueToMagnetic(double trueValue, double variation) {

  double magnetic=trueValue-variation;
  while (magnetic<0) magnetic+=PI_2;
  while (magnetic>=PI_2) magnetic-=PI_2;
  return magnetic;    
}

bool isNMEA0183NaN(double value) {
    return value==NMEA0183DoubleNA;
}