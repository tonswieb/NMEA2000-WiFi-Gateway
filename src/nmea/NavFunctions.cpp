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
    //NMEA0183 NaN is -1E09, but is sometimes converted to a lower value due to 
    //conversion. So need to choose something big enough, but smaller then NMEA0183 NaN.
    return fabs(value) > 1000;
}