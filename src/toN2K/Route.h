/* 
Route.h

Author: Ton Swieb

  This library is free software; you can redistribute it and/or
  modify it as you like.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/
 
#ifndef _Route_H_
#define _Route_H_

#include <Arduino.h>
#include "Log.h"
#include "PrintfWrapper.h"

/*
 * Defaults for Garmin GPS120 / Garmin GPS 12.
 * Overriding using #define does not work on Arduino, so ifndef constructs not used.
 * Use the Route constructor to override the defaults to either save memory or support longer routes / waypoint names.
 */
#define MAX_WP_PER_ROUTE 30
#define MAX_WP_NAME_LENGTH 6

//Defines a waypoint in this route.
struct tRouteWaypoint {

  char* name;
  double latitude;
  double longitude;
};

/**
 * Defines the information needed to send a route from NMEA 0183 to NMEA 2000.
 * Information is received from NMEA 0183 via a sequence of RTE and WPL messages.
 */
class Route {

private:

  tRouteWaypoint* waypoints;
  byte maxWpPerRoute;
  byte maxWpNameLength;
  byte size;
  byte index;
  unsigned int routeID;
  byte indexOriginCurrentLeg = 0; //The index of the waypoint in the route which is the origin of the current leg. 

  Logger* logger;

  /*
   * Is true if the last received RTE message sequence received is equal to the previous received RTE message sequence received.
   * This indicates that all WPL messages received in between the two RTE messages sequence should correspond with these RTE messages.
   * And hence this Route instance should have all information to send a PGN129285 message.
   */
  boolean equalToPrevious = false;

  /*
   * The route is always marked invalid for the duration of processing the RTE sequence.
   * So PGN129285 cannot be sent while processing a new RTE sequence.
   * The route is marked valid as soon as we know the current RTE sequence is equal to previous RTE sequence and hence all 
   * waypoints received in between should also be valid.
   */
  boolean valid = false;

  //Add a waypoint name at the current index. Taking the maximum waypoint length into account.
  void addName(const char* src);
  //True if the waypoint name is equal to the name at the current index. Taking the maximum waypoint lenght into account.
  boolean equalName(const  char* src);
  //True if the waypoint name is equal to the name at the supplied index. Taking the maximum waypoint lenght into account.
  boolean equalName(const  char* src, const byte index);
  void setOriginCurrentLeg(const char* originID);

public:

  //Reinitialize this Route instance to start receiving a new route sequence.
  void initialize(const unsigned int ID);
  
  //Add a new waypoint at the current index as long as maximum waypoints per route is not reached.
  void addWaypoint(const char* src);
  
  //Add a new coordinates at the current index as long as maximum waypoints per route is not reached 
  //and the waypoint name corresponds to the store waypoint name.
  void addCoordinates(const char* src, double latitude, double longitude);

  //Complete the Route instance.
  void finalize(const char type, const char* originID);

  //Log the current set of waypoint names in the route.
  void logWaypointNames();

  tRouteWaypoint getWaypoint(int i) {
    return waypoints[i];
  }
  
  unsigned int getSize() {
    return size;
  }
  boolean isValid() {
    return valid;
  }
  byte getIndexOriginCurrentLeg() {
    return indexOriginCurrentLeg;
  }
  unsigned int getRouteId() {
    return routeID;
  }
  
  Route(byte maxWpPerRoute = MAX_WP_PER_ROUTE, byte maxWpNameLength = MAX_WP_NAME_LENGTH, Logger* logger = 0);
};

#endif
