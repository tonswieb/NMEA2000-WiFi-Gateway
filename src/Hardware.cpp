#include "Hardware.h"

Hardware::Hardware() {

}

Hardware::~Hardware() {

}

void Hardware::begin() {

  FastLED.addLeds<NEOPIXEL, WS2812_PIN>(leds, NUM_LEDS);
  leds[0].blue = 5;
  leds[1].green = 5;
  FastLED.show();
  pinMode(Relais, OUTPUT);
  digitalWrite(Relais, LOW);
  pinMode(MOB, INPUT);
  Wire.begin(26, 25);
  leds[0].blue = 0;
  leds[1].green = 0;
  FastLED.show();
}

void Hardware::end() {

}

void Hardware::loop() {
  if (Blink + 1000 < millis())
  {
    Blink = millis();
    digitalWrite(Relais, !digitalRead(Relais)); //relais test. should be toggeling with 1 Hz
    //make hardbeat
    if(leds[0].green > 1){
      leds[0].green =0;
    }else{
      leds[0].green =5;
    }
    FastLED.show();
    double VCC = analogRead(Vin) * 3.6 * 5.7 / 4095;
    if (_vcc_func) {
        _vcc_func("V:" + String(VCC));
    }
  }
}