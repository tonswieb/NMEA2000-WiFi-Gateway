/* 
N183ToN2k.cpp

2015 Copyright (c) Kave Oy, www.kave.fi  All right reserved.

Author: Timo Lappalainen, Ton Swieb

  This library is free software; you can redistribute it and/or
  modify it as you like.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/
 
#include "N183ToN2k.h"
#include <math.h>

tN2kGNSSmethod GNSMethofNMEA0183ToN2k(int Method) {
  switch (Method) {
    case 0: return N2kGNSSm_noGNSS;
    case 1: return N2kGNSSm_GNSSfix;
    case 2: return N2kGNSSm_DGNSS;
    default: return N2kGNSSm_noGNSS;  
  }
}

N183ToN2k::N183ToN2k(tNMEA2000* pNMEA2000, Gps *gps, Stream* nmea0183, Logger* logger, N2KPreferences* prefs, byte maxWpPerRoute, byte maxWpNameLength) {

  this->prefs = prefs;
  this->logger = logger;
  this->pNMEA2000 = pNMEA2000;
  this->gps = gps;
  this->nav = new Nav(gps,logger);
  route = new Route(maxWpPerRoute, maxWpNameLength, logger);
  info("Initializing NMEA0183 communication. Make sure the NMEA device uses the same baudrate.");
  NMEA0183.SetMessageStream(nmea0183);
  NMEA0183.Open();
}

/**
 * Resend PGN129285 at least every 5 seconds.
 * B&G Triton requires a PGN129285 message around every 10 seconds otherwise display of the destinationID is cleared.
 * Depending on how fast an updated route is constructed we are sending the same or updated route.
 */
void N183ToN2k::handleLoop() {

  tNMEA0183Msg NMEA0183Msg;  
  while (NMEA0183.GetMessage(NMEA0183Msg)) {
    HandleNMEA0183Msg(NMEA0183Msg);
  }
  
  //Send PGN's after the batch of NMEA0183 is processed. Nav data should be updated.
  if (nav->isValid()) {
    sendPGN129283();
    sendPGN129284();
    sendPGN129285();
    nav->reset(); //Reset to prevent resending the same data.
  }

  static unsigned long timeUpdated=millis();
  if (timeUpdated+5000 < millis()) {
    timeUpdated=millis();
    sendPGN129285Route();
  }
}

/**
 * 129283 - Cross Track Error
 * Category: Navigation
 * This PGN provides the magnitude of position error perpendicular to the desired course.
 */
void N183ToN2k::sendPGN129283() {

  tN2kMsg N2kMsg;
  SetN2kXTE(N2kMsg,1,N2kxtem_Autonomous, false, nav->getXte());
  pNMEA2000->SendMsg(N2kMsg);
}

/*
 * 129284 - Navigation Data
 * Category: Navigation
 * This PGN provides essential navigation data for following a route.Transmissions will originate from products that can create and manage routes using waypoints. This information is intended for navigational repeaters. 
 * 
 * Send the navigation information of the current leg of the route.
 * B&G Trition ignores OriginWaypointNumber and DestinationWaypointNumber values in this message.
 * It always takes the 2nd waypoint from PGN129285 as DestinationWaypoint.
 * Not sure if that is compliant with NMEA2000 or B&G Trition specific.
 */
void N183ToN2k::sendPGN129284() {
      //PGN129285 only gives route/wp data ahead in the Active Route. So originID will always be 0 and destinationID will always be 1.
      //Unclear why these ID's need to be set in PGN129284. Needed on B&G Triton displays other values are ignored anyway.      
      struct tETA eta = nav->calcETA();
      tN2kMsg N2kMsg;
      double vmg = nav->getVmg(); //Is calculated if not present in Nav data. So let's retrieve it only once.
      SetN2kNavigationInfo(N2kMsg,1,nav->getDtw(),N2khr_magnetic,nav->isPerpendicularCrossed(),nav->isArrivalCircleEntered(),N2kdct_RhumbLine,eta.etaTime,eta.etaDays,
                          nav->getBotwMagnetic(),nav->getBtwMagnetic(),0,1,nav->getLatitude(),nav->getLongitude(),vmg);
      pNMEA2000->SendMsg(N2kMsg);
      trace("129284: originID=%s, destinationID=%s, latitude=%3.6f, longitude=%3.6f, ArrivalCircleEntered=%c, VMG=%3.1f, DTW=%3.3f, BTW (Current to Destination)=%2.6f, BTW (Orign to Desitination)=%2.6f, ETA (time)=%6.0f, ETA (days)=%i",
      nav->getOriginId(),nav->getDestinationId(),nav->getLatitude(),nav->getLongitude(),nav->isArrivalCircleEntered() ? 'T' : 'F',vmg,nav->getDtw(),nav->getBtwMagnetic(),nav->getBotwMagnetic(),eta.etaTime,eta.etaDays);
}

/**
 * 129285 - Navigation - Route/WP information
 * Category: Navigation
 * This PGN shall return Route and WP data ahead in the Active Route. It can be requested or may be transmitted without a request, typically at each Waypoint advance. 
 * 
 * For navigation systems that do not send RTE and WPL messages, like iSailor.
 * Let's create a route from RMB message with the current position and next waypoint.
 */
void N183ToN2k::sendPGN129285() {

  tN2kMsg N2kMsg;
  //TODO: Check if lat/lon is valid before trying to send N2K message
  double Latitude = gps->getLatitude(); 
  double Longitude = gps->getLongitude();
  SetN2kPGN129285(N2kMsg,0, 1, '1', false, false, "Unknown");
  AppendN2kPGN129285(N2kMsg, 0, nav->getOriginId(), Latitude, Longitude);
  AppendN2kPGN129285(N2kMsg, 1, nav->getDestinationId(), nav->getLatitude(), nav->getLongitude());
  trace("129285: %s,%2.6f,%2.6f",nav->getOriginId(),Latitude,Longitude);
  trace("129285: %s,%2.6f,%2.6f",nav->getDestinationId(),nav->getLatitude(),nav->getLongitude());
  pNMEA2000->SendMsg(N2kMsg);       
}

/**
 * 129285 - Navigation - Route/WP information
 * Category: Navigation
 * This PGN shall return Route and WP data ahead in the Active Route. It can be requested or may be transmitted without a request, typically at each Waypoint advance. 
 * 
 * Send the active route with all waypoints from the origin of the current leg and onwards.
 * So the waypoint that corresponds with the originID from the BOD message should be the 1st. The destinationID from the BOD message should be the 2nd. Etc.
 */
void N183ToN2k::sendPGN129285Route() {

  if (!route->isValid()) {
    debug("Skip sending PGN129285, route is not complete yet.");
    return;
  }

  tN2kMsg N2kMsg;
  SetN2kPGN129285(N2kMsg,0, 1, route->getRouteId(), false, false, "Unknown");

  /*
   * With Garmin GPS 120 GOTO route we only get 1 waypoint. The destination. Perhaps this is the default for all NMEA0183 devices.
   * For NMEA2000 the destination needs to be 2nd waypoint in the route. So lets at the current location as the 1st waypoint in the route.
   */   
  if (route->getSize() == 1) {
    //TODO: Check if lat/lon is valid before trying to send N2K message
    double Latitude = gps->getLatitude();
    double Longitude = gps->getLongitude();
    AppendN2kPGN129285(N2kMsg, 0, "CURRENT", Latitude, Longitude);
    tRouteWaypoint e = route->getWaypoint(0);
    AppendN2kPGN129285(N2kMsg, 1, e.name, e.latitude, e.longitude);
    trace("129285: CURRENT,%2.6f,%2.6f", Latitude, Longitude);
    trace("129285: %s,%2.6f,%2.6f",e.name,e.latitude,e.longitude);
  } else {
    for (byte i=route->getIndexOriginCurrentLeg(); i < route->getSize(); i++) {
      byte j = i - route->getIndexOriginCurrentLeg();
      tRouteWaypoint e = route->getWaypoint(i);
      //Continue adding waypoints as long as they fit within a single message
      trace("129285: %s,%2.6f,%2.6f",e.name,e.latitude,e.longitude);
      if (!AppendN2kPGN129285(N2kMsg, j, e.name, e.latitude, e.longitude)) {
        //Max. nr. of waypoints per message is reached.Send a message with all waypoints upto this one and start constructing a new message.
        pNMEA2000->SendMsg(N2kMsg); 
        N2kMsg.Clear();
        SetN2kPGN129285(N2kMsg,j, 1, route->getRouteId(), false, false, "Unknown");
        //TODO: Check for the result of the Append, should not fail due to message size. Perhaps some other reason?
        AppendN2kPGN129285(N2kMsg, j, e.name, e.latitude, e.longitude);
      }
    }
  }
  pNMEA2000->SendMsg(N2kMsg);       
}

void N183ToN2k::sendPGN129029(const tGGA &gga) {
  
    tN2kMsg N2kMsg;
    SetN2kGNSS(N2kMsg,1,gps->getDaysSince1970(),gga.GPSTime,gga.latitude,gga.longitude,gga.altitude,
              N2kGNSSt_GPS,GNSMethofNMEA0183ToN2k(gga.GPSQualityIndicator),gga.satelliteCount,gga.HDOP,0,
              gga.geoidalSeparation,1,N2kGNSSt_GPS,gga.DGPSReferenceStationID,gga.DGPSAge
              );
    pNMEA2000->SendMsg(N2kMsg); 
}

void N183ToN2k::sendPGN129025(const double &latitude, const double &longitude) {

    tN2kMsg N2kMsg;
    SetN2kLatLonRapid(N2kMsg, latitude, longitude);
    pNMEA2000->SendMsg(N2kMsg);
}

void N183ToN2k::sendPGN129026(const tN2kHeadingReference ref, const double &COG, const double &SOG) {

    tN2kMsg N2kMsg;
    SetN2kCOGSOGRapid(N2kMsg,1,ref,COG,SOG);
    pNMEA2000->SendMsg(N2kMsg);
}

void N183ToN2k::sendPGN127250(const double &trueHeading) {

    tN2kMsg N2kMsg;
    double Variation = gps->getVariation();
    double MHeading = trueToMagnetic(trueHeading,Variation);
    SetN2kMagneticHeading(N2kMsg,1,MHeading,0,Variation);
    pNMEA2000->SendMsg(N2kMsg);
}

// NMEA0183 message Handler functions
void N183ToN2k::HandleRMC(const tNMEA0183Msg &NMEA0183Msg) {

  tRMC rmc;
  if (NMEA0183ParseRMC(NMEA0183Msg,rmc) && rmc.status == 'A') {
    tN2kMsg N2kMsg;
    double MCOG = trueToMagnetic(rmc.trueCOG,rmc.variation);
    sendPGN129026(N2khr_magnetic,MCOG,rmc.SOG);
    sendPGN129025(rmc.latitude,rmc.longitude);
    gps->setCOG(rmc.trueCOG);
    gps->setSOG(rmc.SOG);
    gps->setLatLong(rmc.latitude,rmc.longitude);
    gps->setDaysSince1970(rmc.daysSince1970);
    gps->setVariation(rmc.variation);
    gps->setSecondsSinceMidnight(rmc.GPSTime);
    trace("RMC: GPSTime=%6.0f, Latitude=%2.6f, Longitude=%2.6f, COG=%1.6f, SOG=%2.2f, DaysSince1970=%u, Variation=%1.6f",
    rmc.GPSTime,rmc.latitude,rmc.longitude,rmc.trueCOG,rmc.SOG,rmc.daysSince1970,rmc.variation);
  } else if (rmc.status == 'V') { warn("RMC: Is Void");
  } else { error("RMC: Failed to parse message"); }
}

/**
 * Receive NMEA0183 RMB message (Recommended Navigation Data for GPS).
 * Contains enough information to send a NMEA2000 PGN129283 (XTE) message and NMEA2000 PGN129284 message.
 */
void N183ToN2k::HandleRMB(const tNMEA0183Msg &NMEA0183Msg) {

  tRMB rmb;
  if (NMEA0183ParseRMB(NMEA0183Msg, rmb)  && rmb.status == 'A') {
    nav->setArrivalCircleEntered(rmb.arrivalAlarm == 'A');
    nav->setBtw(rmb.btw, false); //BTW from RMB is always in true.
    nav->setDestinationId(rmb.destID);    
    nav->setDtw(rmb.dtw);
    nav->setXte(rmb.xte);
    nav->setLatLong(rmb.latitude,rmb.longitude);
    nav->setOriginId(rmb.originID);
    nav->setVmg(rmb.vmg);

    trace("RMB: XTE=%6.2f, DTW=%1.6f, BTW=%1.6f, VMG=%2.2f, OriginID=%s, DestinationID=%s, Latittude=%2.6f, Longitude=%2.6f",
    rmb.xte,rmb.dtw,rmb.btw,rmb.vmg,rmb.originID,rmb.destID,rmb.latitude,rmb.longitude);
  } else if (rmb.status == 'V') { warn("RMB: Is Void");
  } else { error("RMB: Failed to parse message"); }
}

void N183ToN2k::HandleGGA(const tNMEA0183Msg &NMEA0183Msg) {

  tGGA gga;
  if (NMEA0183ParseGGA(NMEA0183Msg,gga) && gga.GPSQualityIndicator > 0) {
    sendPGN129029(gga);
    gps->setLatLong(gga.latitude,gga.longitude);
    gps->setSecondsSinceMidnight(gga.GPSTime);
    trace("GGA: Time=%6.0f, Latitude=%2.6f, Longitude=%2.6f, Altitude=%3.2f, GPSQualityIndicator=%u, SatelliteCount=%u, HDOP=%6.2f, GeoidalSeparation=%6.2f, DGPSAge=%6.2f, DGPSReferenceStationID=%u",
    gga.GPSTime,gga.latitude,gga.longitude,gga.altitude,gga.GPSQualityIndicator,gga.satelliteCount,gga.HDOP,gga.geoidalSeparation,gga.DGPSAge,gga.DGPSReferenceStationID);
  } else if (gga.GPSQualityIndicator == 0) { warn("GGA: Invalid GPS fix.");
  } else { error("GGA: Failed to parse message"); }
}

void N183ToN2k::HandleGLL(const tNMEA0183Msg &NMEA0183Msg) {

  tGLL gll;
  if (NMEA0183ParseGLL(NMEA0183Msg,gll) && gll.status == 'A') {
    sendPGN129025(gll.latitude,gll.longitude);
    gps->setLatLong(gll.latitude,gll.longitude);
    gps->setSecondsSinceMidnight(gll.GPSTime);
    trace("GLL: Time=%6.0f, Latitude=%2.6f, Longitude=%2.6f",gll.GPSTime,gll.latitude,gll.longitude);
  } else if (gll.status == 'V') { warn("GLL: Is  Void");
  } else { error("GLL: Failed to parse message"); }
}

void N183ToN2k::HandleHDT(const tNMEA0183Msg &NMEA0183Msg) {

  double TrueHeading;
  if (NMEA0183ParseHDT(NMEA0183Msg,TrueHeading)) {
    sendPGN127250(TrueHeading);
    trace("HDT: True heading=%1.6f",TrueHeading);
  } else { error("HDT: Failed to parse message"); }
}

void N183ToN2k::HandleVTG(const tNMEA0183Msg &NMEA0183Msg) {
 double MagneticCOG, COG, SOG;
  
  if (NMEA0183ParseVTG(NMEA0183Msg,COG,MagneticCOG,SOG)) {
    gps->setCOG(COG);
    gps->setSOG(SOG);
    gps->setVariation(COG-MagneticCOG); // Save variation for Magnetic heading
    sendPGN129026(N2khr_true,COG,SOG);
    trace("VTG: COG=%1.6f",COG);
  } else { error("VTG: Failed to parse message"); }
}

/**
 * Receive NMEA0183 BOD message (Bearing Origin to Destination) and store it for later use when the RMB message is received.
 * Does not contain enough information itself to send a single NMEA2000 message.
 */
void N183ToN2k::HandleBOD(const tNMEA0183Msg &NMEA0183Msg) {

  tBOD bod;
  if (NMEA0183ParseBOD(NMEA0183Msg,bod)) {
    //First set magnetic BOTW because it requires transformation and is sometimes missing.
    //Then override with true BOTW if present. So either one of the 2 is set with a preference for true.
    nav->setBotw(bod.magBearing, true);
    nav->setBotw(bod.trueBearing, false);
    nav->setOriginId(bod.originID);
    nav->setDestinationId(bod.destID);
    trace("BOD: True heading=%1.6f, Magnetic heading=%1.6f, Origin ID=%s, Dest ID=%s",bod.trueBearing,bod.magBearing,bod.originID,bod.destID);
  } else { error("BOD: Failed to parse message"); }
}

/**
 * Handle receiving NMEA0183 RTE message (route).
 * Based on the info which is processed after the last correlated RTE message is received a NMEA2000 messages is periodically sent using a timer.
 * A timer is used because (at least) B&G Triton requires a new PGN129285 around every 10 sec, but receiving RTE messages could take much longer 
 * on routes with more then 5 waypoints. So we need to sent the same PGN129285 multiple times in between.
 * For this we also need previously send WPL, messages. A single WPL message is sent per NMEA0183 message cycle (RMC, RMB, GGA, WPL) followed 
 * by the RTE message(s) in the next cycle. So it can take a while for long routes before alle waypoints are received and finally the RTE messages.
 */
void N183ToN2k::HandleRTE(const tNMEA0183Msg &NMEA0183Msg) {

  tRTE rte;
  if (NMEA0183ParseRTE(NMEA0183Msg,rte)) {

    if (rte.currSentence == 1) {
      route->initialize(rte.routeID);
    }

    //Combine the waypoints of correlated RTE messages in a central list.
    //Will be processed when the last RTE message is recevied.
    for (int i=0; i<rte.nrOfwp; i++) {
      route->addWaypoint(rte[i]);
    }
    if (LOG_TRACE) {
      route->logWaypointNames();
      log_P("RTE: Waypoints in message: ");
      for (byte i=0; i<rte.nrOfwp;i++) {
        log(rte[i]);log_P(",");
      }
      log_P("\n");
    }

    if (rte.currSentence == rte.nrOfsentences) {
      route->finalize(rte.type,nav->getOriginId());
      //Sending PGN129285 is handled from a timer.
    }
    trace("RTE: Time=%u, Nr of sentences=%u, Current sentence=%u, Type=%s, Route ID=%u",millis(),rte.nrOfsentences,rte.currSentence,rte.type,rte.routeID);
  } else { error("RTE: Failed to parse message"); }
}

/**
 * Receive NMEA0183 WPL message (Waypoint List) and store it for later use after all RTE messages are received.
 * A single WPL message is sent per NMEA0183 message cycle (RMC, RMB, GGA, WPL) so it can take a while for long routes 
 * before alle waypoints are received.
 * Does not contain enough information itself to send a single NMEA2000 message.
 */
void N183ToN2k::HandleWPL(const tNMEA0183Msg &NMEA0183Msg) {
  
  tWPL wpl;
  if (NMEA0183ParseWPL(NMEA0183Msg,wpl)) {
    route->addCoordinates(wpl.name, wpl.latitude, wpl.longitude);
  } else { error("WPL: Failed to parse message"); }
}

/**
 * Receive NMEA0183 APB message (Auto Pilot B) and store it for later use when the RMB message is received.
 * Mainly to know if the perpendicular has been crossed at the waypoint.
 * Could be used to send a PNG 129283 (Cross Track Error), but that message is already send from an RMB message.
 * Typically navigation systems send the APB, RMB, BOD messages together in a batch. So let's pick what we need and let's not send to many NMEA02000 messages.
 */
void N183ToN2k::HandleAPB(const tNMEA0183Msg &NMEA0183Msg) {

  tAPB apb;
  if (NMEA0183ParseAPB(NMEA0183Msg, apb)  && apb.status == 'A') {
    nav->setBotw(apb.botw,apb.botwMode == 'M');
    nav->setBtw(apb.btw, apb.btwMode == 'M');
    nav->setHdt(apb.headingToSteer, apb.headingToSteerMode == 'M');
    nav->setXte(apb.xte);
    nav->setperpendicularCrossed(apb.perpendicularPassed == 'A');
    nav->setArrivalCircleEntered(apb.arrivalAlarm == 'A');    
    trace("APB: XTE=%6.2f, BTW =%1.6f, BOD =%1.6f,  ArrivalCircleEntered=%c, DestinationID=%s", apb.xte,apb.btw,apb.botw,apb.perpendicularPassed,apb.destID);
  } else if (apb.status == 'V') { warn("APB: Is Void");
  } else { error("APB: Failed to parse message"); }
}

/*
 * NMEA0183Msg.IsMessageCode wrapper function using PROGMEM Strings to limited SRAM usage.
 */
boolean isMessageCode_P(const tNMEA0183Msg &NMEA0183Msg, const char* code) {
  
  char buffer[4];
  strcpy_P(buffer, code); //Copy from PROGMEM to SRAM
  return NMEA0183Msg.IsMessageCode(buffer);
}

/**
 * iSailor can send APB, BOD, BWC, BWR, HSC, RMB, RMC, XTE
 */
void N183ToN2k::HandleNMEA0183Msg(const tNMEA0183Msg &NMEA0183Msg) {

  //TODO Extract function
  //TODO Support a nmea filter per instance
  std::string messageCode(NMEA0183Msg.MessageCode());
  std::string filter(prefs->getNmeaFilter());
  if (filter.find(messageCode) != std::string::npos) {
    info("Filtering %s",NMEA0183Msg.MessageCode());
    return;
  }

  debug("Handling NMEA0183 message %s",NMEA0183Msg.MessageCode());

  if (logger->getLevel() == DEBUG_LEVEL_TRACE) {
    //We don't have access to the RAW data, so let's reconstruct it from the fields to print it.
    log(NMEA0183Msg.GetPrefix())
    log(NMEA0183Msg.Sender());
    log(NMEA0183Msg.MessageCode());
    for (int i=0; i < NMEA0183Msg.FieldCount();i++) {
      log(",");
      log(NMEA0183Msg.Field(i));
    }
    logln();
  }

  if (isMessageCode_P(NMEA0183Msg,PSTR("GGA"))) {
    HandleGGA(NMEA0183Msg);
  } else if (isMessageCode_P(NMEA0183Msg,PSTR("GGL"))) {
    HandleGLL(NMEA0183Msg);
  } else if (isMessageCode_P(NMEA0183Msg,PSTR("RMB"))) {
    HandleRMB(NMEA0183Msg);
  } else if (isMessageCode_P(NMEA0183Msg,PSTR("RMC"))) {
    HandleRMC(NMEA0183Msg);
  } else if (isMessageCode_P(NMEA0183Msg,PSTR("WPL"))) {
    HandleWPL(NMEA0183Msg);
  } else if (isMessageCode_P(NMEA0183Msg,PSTR("BOD"))) {
    HandleBOD(NMEA0183Msg);
  } else if (isMessageCode_P(NMEA0183Msg,PSTR("VTG"))) {
    HandleVTG(NMEA0183Msg);
  } else if (isMessageCode_P(NMEA0183Msg,PSTR("HDT"))) {
    HandleHDT(NMEA0183Msg);
  } else if (isMessageCode_P(NMEA0183Msg,PSTR("RTE"))) {
    HandleRTE(NMEA0183Msg);
  } else if (isMessageCode_P(NMEA0183Msg,PSTR("APB"))) {
    HandleAPB(NMEA0183Msg);
  }
} 

