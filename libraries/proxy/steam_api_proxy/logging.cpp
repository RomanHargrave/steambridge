// logging.cpp - Handles various logging (and related) utilities
// Copyright (c) 2014 Bryan DeGrendel
//
// See COPYING and license/LICENSE.steambridge for license information

#include "stdafx.h"

#include <cstdarg>
#include <cstdlib>
#include <cstdio>

#include <sstream>
#include <string>

#include "logging.h"
#include "types.h"
#include <config.h>

#define _STREAM stdout
#define _VA_PRINT(ARG) { \
  char temp[2049]; \
  memset(temp, 0, 2049); \
  va_list va; \
  va_start(va, ARG); \
  vsnprintf_s(temp, 2049, 2048, ARG, va); \
  va_end(va); \
  ss << temp; \
}

__declspec(noreturn) void __stub__(const char *func, const char *args, ...)
{
  std::stringstream ss;
  std::stringstream appName;
  appName << _STEAM_BRIDGE_APPNAME << " internal error";
  ss << func;
  _VA_PRINT(args);
  ss << ": is a stub!\n";
  ss << "This means a function is unimplemented, but required by this app.\n";
  fprintf(_STREAM, "ERROR: ");
  fprintf(_STREAM, ss.str().c_str());
  MessageBoxA(NULL,
              ss.str().c_str(),
              appName.str().c_str(),
              MB_ICONERROR | MB_OK);
  exit(1);
}

__declspec(noreturn) void __cant_implement__(const char *func,
    const char *args, ...)
{
  std::stringstream ss;
  std::stringstream appName;
  appName << _STEAM_BRIDGE_APPNAME << " internal error";
  ss << func;
  _VA_PRINT(args);
  ss << ": cannot be implemented!\n";
  ss << "This means a function cannot be implemented, but is required by this app.\n";
  fprintf(_STREAM, "ERROR: ");
  fprintf(_STREAM, ss.str().c_str());
  MessageBoxA(NULL,
              ss.str().c_str(),
              appName.str().c_str(),
              MB_ICONERROR | MB_OK);
  exit(1);
}

void __trace__(const char *func, const char *args, ...)
{
  std::stringstream ss;
  ss << "TRACE: " << func;
  _VA_PRINT(args);
  ss << std::endl;
  fprintf(_STREAM, ss.str().c_str());
}

void __log__(const char *func, const char *msg, ...)
{
  std::stringstream ss;
  ss << "LOG: " << func;
  _VA_PRINT(msg);
  ss << std::endl;
  fprintf(_STREAM, ss.str().c_str());
}

__declspec(noreturn) void __abort__(const char *func, const char *msg, ...)
{
  std::stringstream ss;
  std::stringstream appName;
  appName << _STEAM_BRIDGE_APPNAME << " internal error";
  ss << func;
  _VA_PRINT(msg);
  ss << ": has called abort!\n";
  ss << "This signifies a probable internal error.\n";
  fprintf(_STREAM, "ABORT: ");
  fprintf(_STREAM, ss.str().c_str());
  ss << "Check the command line for more information\n";
  MessageBoxA(NULL, 
              ss.str().c_str(),
              appName.str().c_str(),
              MB_ICONERROR | MB_OK);
  exit(1);
}

