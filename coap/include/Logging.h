/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __Logging_h
#define __Logging_h

#include <iostream>

enum LogLevel {
  LLTRACE = 0,
  LLDEBUG = 1,
  LLINFO = 2,
  LLWARNING = 3,
  LLERROR = 4
};

#define SETLOGLEVEL(LEVEL) static const LogLevel FILE_LOGLEVEL = LEVEL;

#define LOG(LEVEL, PREFIX) (LEVEL >= FILE_LOGLEVEL) && std::cout << __FILE__  << ':' << __LINE__ << ' ' << PREFIX << ' '

#define TLOG LOG(LLTRACE, "(TRACE)")
#define DLOG LOG(LLDEBUG, "(DEBUG)")
#define ILOG LOG(LLINFO, "(INFO)")
#define WLOG LOG(LLWARNING, "(WARNING)")
#define ELOG LOG(LLERROR, "(ERROR)")

#endif //__Logging_h
