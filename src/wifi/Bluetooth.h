#ifndef _BLUETOOTH_GPS_H_
#define _BLUETOOTH_GPS_H_

#include <BluetoothSerial.h>
#include <prefs/N2KPreferences.h>
#include <util/Log.h>

extern HardwareSerial Serial;

class Bluetooth {

private:
  //Internal toggles to check against changed preferences.
  bool blEnabled;
  bool blGpsEnabled;
  BluetoothSerial *SerialBT;
  N2KPreferences *prefs;
  Logger *logger;

  void connectBlGps();
  void disconnectBlGps();

public:
  Bluetooth(BluetoothSerial *bluetoothSerial, N2KPreferences *prefs, Logger* logger);
  void begin();
  void end();
  void sendUdpPackage(char *message);
};

#endif