  #include <nmea/Gps.h>

  void Gps::setLatLong(double latitude, double longitude) {
      this->latitude = latitude;
      this->longitude = longitude;
      lastPositionTime = millis();
  }

  void Gps::setVariation(double variation) {
      this->variation = variation;
  }

  double Gps::getLatitude() {
      return latitude;
  }

  double Gps::getLongitude() {
      return longitude;
  }

  double Gps::getVariation() {
      return variation;
  }

  bool Gps::isValid() {
    return lastPositionTime + EXPERIATION_IN_MILLIS < millis();
  }

  void Gps::setDaysSince1970(unsigned long daysSince1970) {
      this->daysSince1970 = daysSince1970;
  }

  unsigned long Gps::getDaysSince1970() {
      return daysSince1970;
  }

  void Gps::setSecondsSinceMidnight(double secondsSinceMidnight) {
      this->secondsSinceMidnight = secondsSinceMidnight;
  }

  double Gps::getSecondsSinceMidnight() {
      return secondsSinceMidnight;
  }
