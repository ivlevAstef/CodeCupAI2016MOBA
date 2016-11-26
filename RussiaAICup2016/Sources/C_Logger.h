//
//File: Logger.h
//Description:
//Author: Ivlev Alexander. Stef
//Created: 21:21 16/7/2015
//Copyright (c) SIA 2015. All Right Reserved.
//
#pragma once

#include <string>
#include <cstdlib>

///Fatal - log and exit. (use only if can't be corrected data)
///Error - log and if enable debug then exit. (use if can't be corrected data but the application can run)
///Warning - log. (use if can be correct data, but they should not be сorrectly)
///Info - if enable debug then log. (use for log not often code/event)
///Trace - if enable debug and trace then log. (use for unimportant code/event)

///Assert - if conditional then equivalent error, else not log.

namespace AICup
{
  class Logger {
  public:
   static void log(const char* logLevel, const char* file, int line, const char* message, ...);
  };
};


#define LogMsg(MSG, ...) __FILE__, __LINE__, MSG, ##__VA_ARGS__

#define LogFTL(MSG, ...) { AICup::Logger::log("Fatal", LogMsg(MSG, ##__VA_ARGS__)); exit(EXIT_FAILURE); }
#define LogERR(MSG, ...) { AICup::Logger::log("Error", LogMsg(MSG, ##__VA_ARGS__)); }
#define LogWRN(MSG, ...) { AICup::Logger::log("Warning", LogMsg(MSG, ##__VA_ARGS__)); }
#define LogINF(MSG, ...) { AICup::Logger::log("Info", LogMsg(MSG, ##__VA_ARGS__)); }
#define LogTRC(MSG, ...) { AICup::Logger::log("Trace", LogMsg(MSG, ##__VA_ARGS__)); }


#ifdef _DEBUG
  #define LogAssertMsg(CONDITION, MSG, ...) if (!(CONDITION)) { \
      AICup::Logger::log("Assert", LogMsg(MSG, ##__VA_ARGS__)); exit(EXIT_FAILURE); \
    }


#else
  #define LogAssertMsg(CONDITION, MSG, ...)  if (!(CONDITION)) { \
    AICup::Logger::log("Assert", LogMsg(MSG, ##__VA_ARGS__)); \
  }
#endif
#define LogAssert(CONDITION) { LogAssertMsg(CONDITION, #CONDITION" failed."); }

#define LogFatalAssert(CONDITION)  if (!(CONDITION)) { \
    LogFTL(#CONDITION" failed."); \
  }
