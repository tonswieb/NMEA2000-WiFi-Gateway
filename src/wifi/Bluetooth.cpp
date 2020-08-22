#include "Bluetooth.h"

void btCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {

    esp_spp_status_t status;
    if (event == 0) {
        status = param->init.status;
        Serial.println("Bluetooth: SSP initialized.");
    } else if (event == 8) {
        status = param->disc_comp.status;
        Serial.println("Bluetooth: SSP discovery complete.");
    } else if (event == 26) {
        status = param->open.status;
        Serial.println("Bluetooth: SSP opened client Connection: ");
    } else if (event == 27) {
        status = param->close.status;
        Serial.println("Bluetooth: SSP closed client connection.");
    } else if (event == 28) {
        status = param->srv_open.status;
        Serial.println("Bluetooth: SSP server started.");
    } else if (event == 29) {
        status = param->cl_init.status;
        Serial.println("Bluetooth: SSP initiated client connection.");        
    } else if (event == 30) {
        status = param->data_ind.status;
        //Not logging reading data. It will flood the log.
    } else if (event == 31) {
        status = param->cong.status;
        //Not logging congestion. It will flooad the log.
    } else if (event == 33) {
        status = param->write.status;
        //Not logging writing data. It will flood the log.
    } else if (event == 34) {
        status = param->srv_open.status;
        Serial.println("Bluetooth: SSP opened server connection: ");
    }
}

Bluetooth::Bluetooth(BluetoothSerial *bluetoothSerial,N2KPreferences *prefs, Logger* logger)
{
    this->SerialBT = bluetoothSerial;
    this->logger = logger;
    this->prefs = prefs;
    prefs->registerCallback([&]() {
        begin();
        connectBlGps();
        disconnectBlGps();
        end();
    });
}

void Bluetooth::begin() {

  if (prefs->isBlEnabled() && !blEnabled) {
    blEnabled = true;
    info("Initializing bluetooth.");
    SerialBT->begin("N2K-bridge", true);
  }
  connectBlGps();
}

void Bluetooth::end() {
    disconnectBlGps();
    if (!prefs->isBlEnabled() && blEnabled) {
        info("Disable Bluetooth");
        blEnabled = false;
        SerialBT->end();
    }
}

void Bluetooth::sendUdpPackage(char *buf)
{
  if (blEnabled && prefs->isNmeaToBluetooth()) {
    SerialBT->println(buf);
  }
}

void Bluetooth::connectBlGps() {

    if (!blGpsEnabled && prefs->isNmeaSrcBlGPSEnabled()) {
        blGpsEnabled = true;
        SerialBT->register_callback(&btCallback);
        SerialBT->connect("XGPS160-3F6E60");
    }
}

void Bluetooth::disconnectBlGps() {
    if (blGpsEnabled && !prefs->isNmeaSrcBlGPSEnabled()) {
        blGpsEnabled = false;
        SerialBT->disconnect();
    }
}