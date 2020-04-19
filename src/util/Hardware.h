#ifndef _HARDWARE_H_
#define _HARDWARE_H_

#define NUM_LEDS 2   //number of WS8212B leds
#define WS2812_PIN 4 //ouput to pin WS2812
#define Vin 37       //Board voltage measring input
#define MOB 38       //man over board input
#define Relais 0     //ouput extarnal power (12V 500mA max)

// #include <Arduino.h>
#include <Wire.h>
#include <FastLED.h>
#include <driver/rtc_io.h> //needed for using the ESP-PICO-D4 IO pins
#include <functional>

class Hardware
{

protected:
    long Blink;
    CRGB leds[NUM_LEDS];
    std::function<void (String)> _vcc_func;

public:
  Hardware();
  ~Hardware();
  void begin();
  void handleVCCUpdate(std::function<void (String)> vcc_func) {
      _vcc_func = vcc_func;
  }
  void end();
  void loop();
};

#endif