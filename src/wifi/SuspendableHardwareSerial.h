#ifndef SuspendableHardwareSerial_h
#define SuspendableHardwareSerial_h

#include <HardwareSerial.h>
#include <functional>

class SuspendableHardwareSerial: public HardwareSerial {

protected:
    Stream *logger;
    bool initialized = false;
    //Let's start suspended until preferences are initialized
    bool suspended = true;
    unsigned long baud;
    uint32_t config;
    int8_t rxPin;
    int8_t txPin;
    bool invert;
    unsigned long timeout_ms;

public:
    SuspendableHardwareSerial(int uart, Stream *logger);
    void begin(unsigned long baud, uint32_t config=SERIAL_8N1, int8_t rxPin=-1, int8_t txPin=-1, bool invert=false, unsigned long timeout_ms = 20000UL);
    void suspend(bool value);
};
#endif
