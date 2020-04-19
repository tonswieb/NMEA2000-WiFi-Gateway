/* 
Route.cpp

Author: Ton Swieb

  This library is free software; you can redistribute it and/or
  modify it as you like.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/
 
#include "PrintfWrapper.h"

PrintfWrapper::PrintfWrapper(Stream* stream, int bufferLength) : stream(stream), bufferLength(bufferLength) {}

void PrintfWrapper::printf(char *fmt, ... ){
  
  va_list args;
  va_start (args, fmt );
  printf(fmt,args);
  va_end (args);
}

void PrintfWrapper::printfln(char *fmt, ... ){
  
  va_list args;
  va_start (args, fmt );
  printf(fmt,args);
  va_end (args);
  stream->print("\n");
}

void PrintfWrapper::printf(char *fmt, va_list args) {
  
  char buf[bufferLength]; // resulting string limited to 128 chars
  vsnprintf(buf, bufferLength, fmt, args);
  stream->print(buf);
}

void PrintfWrapper::printfln(char *fmt, va_list args) {

  printf(fmt,args);
  stream->print("\n");
}


void PrintfWrapper::printf_P(const __FlashStringHelper *fmt, ... ) {
  va_list args;
  va_start (args, fmt);
  printf_P(fmt, args);
  va_end(args);
}
  
void PrintfWrapper::printfln_P(const __FlashStringHelper *fmt, ... ) {
  
  va_list args;
  va_start (args, fmt);
  printf_P(fmt, args);
  va_end(args);
  stream->print("\n");
}

void PrintfWrapper::printf_P(const __FlashStringHelper *fmt, va_list args ) {
  
  char buf[bufferLength]; // resulting string limited to 128 chars
#ifdef __AVR__
  vsnprintf_P(buf, sizeof(buf), (const char *)fmt, args); // progmem for AVR
#else
  vsnprintf(buf, sizeof(buf), (const char *)fmt, args); // for the rest of the world
#endif
  stream->print(buf);
}

void PrintfWrapper::printfln_P(const __FlashStringHelper *fmt, va_list args ) {

  printf_P(fmt,args);
  stream->print("\n");
}
