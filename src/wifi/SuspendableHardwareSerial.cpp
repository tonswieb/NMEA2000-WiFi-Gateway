#include "SuspendableHardwareSerial.h"

SuspendableHardwareSerial::SuspendableHardwareSerial(int uart_nr, Stream *logger) : HardwareSerial(uart_nr) {
    this->logger = logger;
}


void SuspendableHardwareSerial::begin(unsigned long baud, uint32_t config, int8_t rxPin, int8_t txPin, bool invert, unsigned long timeout_ms) {
    initialized = true;
    if (!suspended) {
        logger->print("Initialize Serial ");
        logger->println(_uart_nr);
        HardwareSerial::begin(baud,config,rxPin,txPin,invert,timeout_ms);
    }
    this->baud = baud;
    this->config = config;
    this->rxPin = rxPin;
    this->txPin = txPin;
    this->invert = invert;
    this->_timeout = timeout_ms;
}

void SuspendableHardwareSerial::suspend(bool value) {

    if (!initialized){
        logger->println("begin(...) must be called before we can call suspend(..).");
        return;
    }

    if (value && !suspended) {
        logger->print("Disable Serial");
        logger->println(_uart_nr);
        suspended = value;
        end();
    } else if (!value && suspended ) {
        logger->print("Enable Serial");
        logger->println(_uart_nr);
        suspended = value;    
        begin(baud,config,rxPin,txPin,invert,timeout_ms);   
    }
}