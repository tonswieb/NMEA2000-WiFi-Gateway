#ifndef _BLUETOOTH_GPS_H_
#define _BLUETOOTH_GPS_H_

#include <BluetoothSerial.h>
#include <prefs/N2KPreferences.h>

class BluetoothGps {

private:
  //Internal toggles to check against changed preferences.
  bool blEnabled;
  bool blGpsEnabled;
  BluetoothSerial SerialBT;
  N2KPreferences *prefs;

  void connectBlGps();
  void disconnectBlGps();

public:
  // BluetoothGps(BluetoothSerial *bluetoothSerial, N2KPreferences *prefs);
  BluetoothGps(N2KPreferences *prefs);
  void begin();
  void end();
  void loop();
  void sendUdpPackage(char *message);
};

#endif