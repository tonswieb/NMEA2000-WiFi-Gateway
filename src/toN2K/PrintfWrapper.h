/* 
Route.h

Author: Ton Swieb

  This library is free software; you can redistribute it and/or
  modify it as you like.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/
 
#ifndef _PrintfWrapper_H_
#define _PrintfWrapper_H_

#include <Arduino.h>
#include <stdarg.h>

#define BUFFER_LENGTH 128

class PrintfWrapper {

private:

  Stream* stream;
  int bufferLength;

public:
  void printf(char *fmt, ... );
  void printf(char *fmt, va_list args );
  void printfln(char *fmt, ... );
  void printfln(char *fmt, va_list args );
  void printf_P(const __FlashStringHelper *fmt, ... );
  void printf_P(const __FlashStringHelper *fmt, va_list args );
  void printfln_P(const __FlashStringHelper *fmt, ... );
  void printfln_P(const __FlashStringHelper *fmt, va_list args );
  PrintfWrapper(Stream* stream, int bufferLength = BUFFER_LENGTH);
};

#endif
