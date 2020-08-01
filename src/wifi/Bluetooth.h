#ifndef _BLUETOOTH_GPS_H_
#define _BLUETOOTH_GPS_H_

#include <BluetoothSerial.h>
#include <prefs/N2KPreferences.h>

class Bluetooth {

private:
  //Internal toggles to check against changed preferences.
  bool blEnabled;
  bool blGpsEnabled;
  BluetoothSerial *SerialBT;
  N2KPreferences *prefs;

  void connectBlGps();
  void disconnectBlGps();

public:
  Bluetooth(BluetoothSerial *bluetoothSerial, N2KPreferences *prefs);
  void begin();
  void end();
  void sendUdpPackage(char *message);
};

#endif