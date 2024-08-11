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
  //Nr of days to go until arrival
  long etaDays = N2kInt16NA;
  //ETA Time is second sinds midnight in UTC. 
  double etaTime = N2kDoubleNA;
};

/**
 * Holds the information received from a navigation device (plotter).
 * This class holds and normalizes this data so it can be used to periodically send 
 * NMEA2000 navigation messages.
 * Navigation devices use sentences like: APB, XTE, BOD, RMB, but it is device depended which messages 
 * they send and per message if all fields are set.
 * For example:
 * iSailor can send APB, BOD, BWC, BWR, HSC, RMB, RMC and XTE messages.
 * qtvlm omits only APB, RMB and XTE messages.
 * iSailor omits BOD message with only true BOD, so omitting magnetic BOD.
 * iSailor omits RMB message without VMG and the precision of DTW is limited to 1 decimal NM, so 185 meters.
 * of data. 
 */
class Nav {

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
  //Calculate VMG to destination waypoint based on GPS data (COG/SOG) and BTW.
  double calcVmg();

public:
  Nav(Gps *gps, Logger* logger);
  /**
   * Set Lat/Lon of destination, if valid.
   * return <b>true></b> if valid, <b>false</b> otherwise.
   */
  bool setLatLong(double latitude, double longitude);
  double getLatitude();
  double getLongitude();
  /**
   * Set BTW, bearing from current position to destination, if valid. Either in 
   * magnetic or true.
   * return <b>true></b> if valid, <b>false</b> otherwise.
   */
  bool setBtw(double btw, bool magnetic);
  double getBtwMagnetic();
  double getBtwTrue();
  /**
   * Set BOTW, bearing from origin to destination, if valid. Either in 
   * magnetic or true.
   * return <b>true></b> if valid, <b>false</b> otherwise.
   */
  bool setBotw(double botw, bool magnetic);
  double getBotwMagnetic();
  double getBotwTrue();
  /**
   * Set HDT, heading to steer, if valid. Either in 
   * magnetic or true.
   * return <b>true></b> if valid, <b>false</b> otherwise.
   */
  bool setHdt(double hdt, bool magnetic);
  double getHdtwMagnetic();
  double getHdtTrue();
  /**
   * Set DTW, distance to waypoint (meters), if valid.
   * return <b>true></b> if valid, <b>false</b> otherwise.
   */
  bool setDtw(double dtw);
  /**
   * Get the DTW, distance to waypoint (meters) or N2K_NA in case it is not set..
   */
  double getDtw();
  /**
   * Set VMG (m/s), velocity made good to waypoint, if valid.
   * return <b>true></b> if valid, <b>false</b> otherwise.
   */
  bool setVmg(double vmg);
  /**
   * Get VMG (m/s) either directly as received from navigation data or otherwise calculate it
   * based on GPS data and BTW.
   */
  double getVmg();
  void setArrivalCircleEntered(bool arrivalCircleEntered);
  bool isArrivalCircleEntered();
  void setperpendicularCrossed(bool perpendicularCrossed);
  bool isPerpendicularCrossed();
  void setOriginId(const char* originId);
  char* getOriginId();
  void setDestinationId(const char* destinationId);
  char* getDestinationId();
  /**
   * Set XTE, cross tracke error, if valid, including direction to steer.
   * The direction to steer is left for xte < 0 and right for xte > 0.
   * return <b>true></b> if valid, <b>false</b> otherwise.
   */
  bool setXte(double xte);
  double getXte();
  /**
   * Calculate Estimated Time of Arrival (ETA) based on DTW (meters) / VMG (meters/second).
   * Set ETA to NaN in case either VMG is missing, too small or DTW is missing.
   * ETA is relative to GMT (UTC). At least at B&G Triton2.
   * So need to apply a localtime offset in case the repeater is configured with a localtime offset as well.
   */
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
