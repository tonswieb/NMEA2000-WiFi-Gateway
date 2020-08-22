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

void Logger::setLevel(int level) {
  debugLevel = level;
}

#ifndef _AVR_
void Logger::logError(char *fmt, ...) {

  if (debugLevel >= DEBUG_LEVEL_ERROR) {
    logger->print("ERROR:");
    va_list args;
    va_start (args, fmt);
    printfWrapper->printfln(fmt, args);
    va_end(args);      
  }
}
void Logger::logWarn(char *fmt, ...) {
  
  if (debugLevel >= DEBUG_LEVEL_WARN) {
    logger->print("WARN :");
    va_list args;
    va_start (args, fmt);
    printfWrapper->printfln(fmt, args);
    va_end(args);      
  }
}
void Logger::logInfo(char *fmt, ...) {

  if (debugLevel >= DEBUG_LEVEL_INFO) {
    logger->print("INFO :");
    va_list args;
    va_start (args, fmt);
    printfWrapper->printfln(fmt, args);
    va_end(args);      
  }
}
void Logger::logDebug(char *fmt, ...) {
  
  if (debugLevel >= DEBUG_LEVEL_DEBUG) {
    logger->print("DEBUG:");
    va_list args;
    va_start (args, fmt);
    printfWrapper->printfln(fmt, args);
    va_end(args);      
  }
}
void Logger::logTrace(char *fmt, ...) {
  
  if (debugLevel >= DEBUG_LEVEL_TRACE) {
    logger->print("TRACE:");
    va_list args;
    va_start (args, fmt);
    printfWrapper->printfln(fmt, args);
    va_end(args);      
  }
}
#else
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
#endif


