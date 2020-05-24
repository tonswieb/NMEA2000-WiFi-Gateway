#include "BluetoothGps.h"

void btCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {

  Serial.print("Event: ");Serial.println(event);
}

BluetoothGps::BluetoothGps(N2KPreferences *prefs)
{
    this->prefs = prefs;
    prefs->registerCallback([&]() {
        begin();
        connectBlGps();
        disconnectBlGps();
        end();
    });
}

void BluetoothGps::begin() {

  if (prefs->isBlEnabled() && !blEnabled) {
    blEnabled = true;
    Serial.println("Initializing bluetooth.");
    SerialBT.begin("N2K-bridge", true);
  }
  connectBlGps();
}

void BluetoothGps::end() {
    disconnectBlGps();
    if (!prefs->isBlEnabled() && blEnabled) {
        Serial.println("Disable Bluetooth");
        blEnabled = false;
        SerialBT.end();
    }
}

void BluetoothGps::loop() {
    if (blGpsEnabled) {
        while(SerialBT.available()) {
            //TODO: Send it to different sources!
            SerialBT.read();
        }
    }
}

void BluetoothGps::sendUdpPackage(char *buf)
{
  if (blEnabled && prefs->isNmeaToBluetooth()) {
    // SerialBT.println(buf);
  }
}

void BluetoothGps::connectBlGps() {

    if (blGpsEnabled && prefs->isBlGPSEnabled()) {
        blGpsEnabled = true;
        SerialBT.register_callback(&btCallback);
        SerialBT.connect("XGPS160-3F6E60");
    }
}

void BluetoothGps::disconnectBlGps() {
    if (blGpsEnabled && !prefs->isBlGPSEnabled()) {
        blGpsEnabled = false;
        SerialBT.disconnect();
    }
}