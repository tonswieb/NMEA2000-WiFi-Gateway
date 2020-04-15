/* 
Route.cpp

Author: Ton Swieb

  This library is free software; you can redistribute it and/or
  modify it as you like.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/
 
#include "Log.h"

Logger::Logger(Stream* logger, int debugLevel) : logger(logger), debugLevel(debugLevel) {

  printfWrapper = new PrintfWrapper(logger,256);
}

int Logger::getLevel() {
  return debugLevel;
}

Stream* Logger::getStream() {
  return logger;
}

void Logger::logError(const __FlashStringHelper *fmt, ...) {

  if (debugLevel >= DEBUG_LEVEL_ERROR) {
    logger->print(F("ERROR:"));
    va_list args;
    va_start (args, fmt);
    printfWrapper->printfln_P(fmt, args);
    va_end(args);      
  }
}
void Logger::logWarn(const __FlashStringHelper *fmt, ...) {
  
  if (debugLevel >= DEBUG_LEVEL_WARN) {
    logger->print(F("WARN :"));
    va_list args;
    va_start (args, fmt);
    printfWrapper->printfln_P(fmt, args);
    va_end(args);      
  }
}
void Logger::logInfo(const __FlashStringHelper *fmt, ...) {

  if (debugLevel >= DEBUG_LEVEL_INFO) {
    logger->print(F("INFO :"));
    va_list args;
    va_start (args, fmt);
    printfWrapper->printfln_P(fmt, args);
    va_end(args);      
  }
}
void Logger::logDebug(const __FlashStringHelper *fmt, ...) {
  
  if (debugLevel >= DEBUG_LEVEL_DEBUG) {
    logger->print(F("DEBUG:"));
    va_list args;
    va_start (args, fmt);
    printfWrapper->printfln_P(fmt, args);
    va_end(args);      
  }
}
void Logger::logTrace(const __FlashStringHelper *fmt, ...) {
  
  if (debugLevel >= DEBUG_LEVEL_TRACE) {
    logger->print(F("TRACE:"));
    va_list args;
    va_start (args, fmt);
    printfWrapper->printfln_P(fmt, args);
    va_end(args);      
  }
}

char* Logger::doubleToString(double   __val, signed char   __width, unsigned char   __prec) {
  //Floating-point numbers can be as large as 3.4028235E+38 and as low as -3.4028235E+38. They are stored as 32 bits (4 bytes) of information.
  //loats have only 6-7 decimal digits of precision. That means the total number of digits, not the number to the right of the decimal point.
  //Unlike other platforms, where you can get more precision by using a double (e.g. up to 15 digits), on the Arduino, double is the same size as float.
  char buffer[16]; //Max 7 digits & - sign & decimal point. Assuming that we are not using very large numbers.
  return dtostrf(__val,__width,__prec,buffer);
}



