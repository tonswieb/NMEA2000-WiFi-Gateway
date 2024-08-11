#ifndef _Nav_H_
#define _Nav_H_

#include <Arduino.h>
#include <math.h>
#include "util/Log.h"
#include "nmea/Gps.h"
#include "nmea/NavFunctions.h"
#include <NMEA0183Messages.h>
#include <NMEA2000.h>


struct tETA {
  //ETA is relative to GMT (UTC). At least at B&G Triton2.
  //So need to apply a localtime offset in case the repeater is configured with a localtime offset as well.

  //Nr of days to go. Use a max number to display --:-- on B&G Triton
  long etaDays = LONG_MAX;
  //ETA Time is second sinds midnight in UTC. 
  double etaTime = 0;
};

/**
 * Holds the information received from a navigation device (plotter) in NMEA0183 format.
 * Navigation devices about different sentences, APB, XTE, BOD, RMB with a differing subsets 
 * of data. This class holds and normalizes this data so it can be used to periodically send 
 * NMEA2000 navigation messages.
 */

//iSailor omits BOD Magnetic in BOD message. So compare to a value bigger to check for NaN.
//iSailor omits VMG in RMB message. So compare to a big value to check for NaN.

class Nav {

public:
  const static int EXPERIATION_IN_MILLIS = 1000;

protected:
  Logger* logger;
  //Reference to the GPS data to get the variation for true/magnetic conversion.
  Gps *gps;
  //True bearing from current position to destination waypoint
  double btw = NAN;
  //True bearing from original waypoint to destination waypoint
  double botw = NAN;
  //True heading to steer to destination waypoint.
  double hdt = NAN;
  //Range to destination in nautical miles
  double dtw = NAN;
  //Destination closing velocity in knots
  double vmg = NAN;
  //Latitude of destination waypoint.
  double latitude = NAN;
  //Longitued of destination waypoint.
  double longitude = NAN;
  //Arrival circle entered at destination waypoint
  bool arrivalCircleEntered = false;
  //Perpendicular passed at destination waypoint
  bool perpendicularCrossed = false;
	//Origin waypoint ID
  char originID[NMEA0183_MAX_WP_NAME_LENGTH];
	//Destination waypoint ID
  char destID[NMEA0183_MAX_WP_NAME_LENGTH];
  //Cross Track error - nautical miles
  double crossTrackError = NAN;
  double calcVmg();

public:
  Nav(Gps *gps, Logger* logger);
  bool setLatLong(double latitude, double longitude);
  double getLatitude();
  double getLongitude();
  bool setBtw(double btw, bool magnetic);
  double getBtwMagnetic();
  double getBtwTrue();
  bool setBotw(double botw, bool magnetic);
  double getBotwMagnetic();
  double getBotwTrue();
  bool setHdt(double hdt, bool magnetic);
  double getHdtwMagnetic();
  double getHdtTrue();
  bool setDtw(double dtw);
  double getDtw();
  bool setVmg(double vmg);
  double getVmg();
  void setArrivalCircleEntered(bool arrivalCircleEntered);
  bool isArrivalCircleEntered();
  void setperpendicularCrossed(bool perpendicularCrossed);
  bool isPerpendicularCrossed();
  void setOriginId(const char* originId);
  char* getOriginId();
  void setDestinationId(const char* destinationId);
  char* getDestinationId();
  bool setXte(double xte);
  double getXte();
  tETA calcETA();

  /**
   * Verify if the navigation data is valid and can be used to send navigation PGN's.
   * return <b>true</b> if at least latitude, longitude, xte and btw are valid, <b>false</b> otherwise.
   */
  bool isValid();
  /**
   * Reset all navigation data. To ensure we start clean when receiving the next batch of navigation data messages 
   * and we don't send any obsolete or partially obsolete data.
   */
  void reset();
};

#endif
