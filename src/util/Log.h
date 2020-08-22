/* 
Log.h

Author: Ton Swieb

  This library is free software; you can redistribute it and/or
  modify it as you like.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/
 
#ifndef _Log_H_
#define _Log_H_

#include <Arduino.h>
#include "PrintfWrapper.h"

#define DEBUG_LEVEL_TRACE 4
#define DEBUG_LEVEL_DEBUG 3
#define DEBUG_LEVEL_INFO 2
#define DEBUG_LEVEL_WARN 1
#define DEBUG_LEVEL_ERROR 0

//Some macros to make the logging systeem a bit less cumbersome
#define LOG_TRACE logger!=0 && logger->getLevel() >= DEBUG_LEVEL_TRACE
#define LOG_DEBUG logger!=0 && logger->getLevel() >= DEBUG_LEVEL_DEBUG
#define LOG_INFO logger!=0 && logger->getLevel() >= DEBUG_LEVEL_INFO
#define LOG_WARN logger!=0 && logger->getLevel() >= DEBUG_LEVEL_WARN
#define LOG_ERROR logger!=0 && logger->getLevel() >= DEBUG_LEVEL_ERROR

#define log(message) if (logger!=0) {logger->getStream()->print(message);}
#define logln(message) if (logger!=0) {logger->getStream()->println(message);}
#define log_P(message) if (logger!=0) {logger->getStream()->print(F(message));}
#define logln_P(message) if (logger!=0) {logger->getStream()->println(F(message));}

#ifdef __AVR__
  #define error(fmt, ...) if (logger!=0) {logger->logError(F(fmt), ##__VA_ARGS__);}
  #define warn(fmt, ...) if (logger!=0) {logger->logWarn(F(fmt), ##__VA_ARGS__);}
  #define info(fmt, ...) if (logger!=0) {logger->logInfo(F(fmt), ##__VA_ARGS__);}
  #define debug(fmt, ...) if (logger!=0) {logger->logDebug(F(fmt), ##__VA_ARGS__);}
  #define trace(fmt, ...) if (logger!=0) {logger->logTrace(F(fmt), ##__VA_ARGS__);}
#else
  #define error(fmt, ...) if (logger!=0) {logger->logError(fmt, ##__VA_ARGS__);}
  #define warn(fmt, ...) if (logger!=0) {logger->logWarn(fmt, ##__VA_ARGS__);}
  #define info(fmt, ...) if (logger!=0) {logger->logInfo(fmt, ##__VA_ARGS__);}
  #define debug(fmt, ...) if (logger!=0) {logger->logDebug(fmt, ##__VA_ARGS__);}
  #define trace(fmt, ...) if (logger!=0) {logger->logTrace(fmt, ##__VA_ARGS__);}
#endif

class Logger {

  private:
    Stream* logger;
    byte debugLevel = DEBUG_LEVEL_WARN;
    PrintfWrapper* printfWrapper;

  public:
    int getLevel();
    void setLevel(int level);
    Stream* getStream();
  #ifdef _AVR_
    void logError(const __FlashStringHelper *fmt, ...);
    void logWarn(const __FlashStringHelper *fmt, ...);
    void logInfo(const __FlashStringHelper *fmt, ...);
    void logDebug(const __FlashStringHelper *fmt, ...);
    void logTrace(const __FlashStringHelper *fmt, ...);
  #else
    void logError(char *fmt, ...);
    void logWarn(char *fmt, ...);
    void logInfo(char *fmt, ...);
    void logDebug(char *fmt, ...);
    void logTrace(char *fmt, ...);
  #endif
    Logger (Stream* logger);
};

#endif

