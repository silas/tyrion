/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include <algorithm>
#include "level.h"

namespace tyrion {

std::string LevelToString(Level level) {
  static const char* const buffer[] = {"DEBUG", "INFO", "WARNING", "ERROR",
                                       "CRITICAL"};
  return buffer[level];
}

Level StringToLevel(std::string level, Level default_level) {
  std::transform(level.begin(), level.end(), level.begin(), ::tolower);

  if (level == "critical") {
    return CRITICAL;
  } else if (level == "error") {
    return ERROR;
  } else if (level == "warning") {
    return WARNING;
  } else if (level == "info") {
    return INFO;
  } else if (level == "debug") {
    return DEBUG;
  } else {
    return default_level;
  }
}

}  // namespace tyrion
