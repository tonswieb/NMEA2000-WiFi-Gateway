  #include <nmea/Nav.h>

  Nav::Nav(Gps *gps, Logger* logger) {
    this->gps = gps;
    this->logger = logger;
  }

  bool Nav::isValid() {
    return !isnan(latitude) && !isnan(longitude) && !isnan(btw) && !isnan(botw) && !isnan(crossTrackError);
  }

  void Nav::reset() {
    btw = NAN;
    botw = NAN;
    hdt = NAN;
    dtw = NAN;
    vmg = NAN;
    latitude = NAN;
    longitude = NAN;
    crossTrackError = NAN;
    arrivalCircleEntered = false;
    perpendicularCrossed = false;
  }

  bool Nav::setLatLong(double latitude, double longitude) {
      if (isNMEA0183NaN(latitude) || isNMEA0183NaN(longitude)) {
        return false;
      }
      this->latitude = latitude;
      this->longitude = longitude;
      return true;
  }
  double Nav::getLatitude() {
      return isnan(latitude) ? N2kDoubleNA : latitude;
  }
  double Nav::getLongitude() {
      return isnan(longitude) ? N2kDoubleNA : longitude;
  }
  bool Nav::setBtw(double btw, bool magnetic) {
    if (isNMEA0183NaN(btw)) {
        return false;
    }
    this->btw = magnetic? magneticToTrue(btw, gps->getVariation()) : btw;
    return true;
  }
  double Nav::getBtwMagnetic() {
    return isnan(btw) ? N2kDoubleNA : trueToMagnetic(btw, gps->getVariation());
  }
  double Nav::getBtwTrue(){
    return isnan(btw) ? N2kDoubleNA : btw;
  }
  bool Nav::setBotw(double botw, bool magnetic) {
    if (isNMEA0183NaN(botw)) {
        return false;
    }
    this->botw = magnetic? magneticToTrue(botw, gps->getVariation()) : botw;
    return true;
  }
  double Nav::getBotwMagnetic() {
    return isnan(botw) ? N2kDoubleNA : trueToMagnetic(botw, gps->getVariation());
  }
  double Nav::getBotwTrue() {
    return isnan(botw) ? N2kDoubleNA : botw;
  }
  bool Nav::setHdt(double hdt, bool magnetic) {
    if (isNMEA0183NaN(hdt)) {
        return false;
    }
    this->hdt = magnetic? magneticToTrue(hdt, gps->getVariation()) : hdt;
    return true;
  }
  double Nav::getHdtwMagnetic() {
    return isnan(hdt) ? N2kDoubleNA : trueToMagnetic(hdt, gps->getVariation());
  }
  double Nav::getHdtTrue(){
    return isnan(hdt) ? N2kDoubleNA : hdt;
  }
  bool Nav::setDtw(double dtw) {
    if (isNMEA0183NaN(dtw)) {
        return false;
    }
    this->dtw = dtw;
    return true;
  }
  double Nav::getDtw() {
    return isnan(dtw) ? N2kDoubleNA : dtw;
  }
  bool Nav::setVmg(double vmg) {
    if (isNMEA0183NaN(vmg)) {
        return false;
    }
    this->vmg = vmg;
    return true;
  }
  double Nav::getVmg() {
    return isnan(vmg) ? calcVmg() : vmg;
  }
  void Nav::setArrivalCircleEntered(bool arrivalCircleEntered) {
    this->arrivalCircleEntered = arrivalCircleEntered;
  }
  bool Nav::isArrivalCircleEntered() {
    return arrivalCircleEntered;
  }
  void Nav::setperpendicularCrossed(bool perpendicularCrossed) {
    this->perpendicularCrossed = perpendicularCrossed;
  }
  bool Nav::isPerpendicularCrossed() {
    return perpendicularCrossed;
  }
  void Nav::setOriginId(const char* originId) {
    strncpy(this->originID,originId,NMEA0183_MAX_WP_NAME_LENGTH);
  }

  char* Nav::getOriginId() {
    return originID;
  }
  void Nav::setDestinationId(const char* destinationId) {
    strncpy(this->destID,destinationId,NMEA0183_MAX_WP_NAME_LENGTH);
  }
  char* Nav::getDestinationId() {
    return destID;
  }
  bool Nav::setXte(double xte) {
    if (isNMEA0183NaN(xte)) {
        return false;
    }
    this->crossTrackError = xte;
    return true;
  }
  double Nav::getXte() {
    return isnan(crossTrackError) ? N2kDoubleNA : crossTrackError;
  }

double Nav::calcVmg() {

  double cog = gps->getCOG();
  //TODO: Verify is SOG in m/s, because VMG from RMB message is.
  double sog = gps->getSOG();
  if (isnan(btw) || isnan(sog) || isnan(cog) || sog < 0.01) {
    trace("calcVmg: Skip calculating VMG. Either BTW, SOG or COG is NAN or SOG too small")
    return N2kDoubleNA;
  }
  //TODO: Verify if COG is in true as well or is this in magnetic? btw is default in true!
  double vmg = sog * cos(btw - cog); //Velocity Made good on Course
  trace("calcVmg: VMG=%3.4f, SOG=%3.4f, COG=%2.6f, BTW=%2.6f",vmg, sog, cog, btw);
  return vmg;
}
tETA Nav::calcETA() {

  struct tETA eta;
  double vmc = getVmg();

  //Disable ETA/TTG when vmg is too small or missing
  if (isnan(vmc) || isnan(dtw) || vmc < 0.01) {
    eta.etaTime = N2kDoubleNA;
    eta.etaDays = N2kInt16NA;
    trace("calcETA: Skip calculating ETA. VMC < 0.01 or VMC/DTW is missing.")
  } else {
    double TTG = dtw / vmc;
    double GPSTime = gps->getSecondsSinceMidnight();
    double ETA = TTG + GPSTime;
    eta.etaDays = ETA / (3600 * 24);
    eta.etaTime = ETA - eta.etaDays * 3600 * 24; //in seconds
    trace("calcETA: GPSTime (seconds)=%6.0f, TTG (seconds)=%6.0f, ETA=%6.0f, ETA (time)=%6.0f, ETA (days)=%i",GPSTime,TTG,ETA,eta.etaTime,eta.etaDays);
  }
  return eta;
}