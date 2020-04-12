#include <Arduino.h>
#include <FastLED.h>

#define NUM_LEDS 2 //number of used leds
#define WS2812_PIN 4 //port pin where the led is connect to

extern bool NMEA2000Data;
extern bool NMEA4800Data;
extern bool NMEA38400Data;
extern double VCC;
extern CRGB leds[NUM_LEDS];