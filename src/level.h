/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef TYRION_LEVEL_H_
#define TYRION_LEVEL_H_

#include <string>

namespace tyrion {

enum Level {
  DEBUG,
  INFO,
  WARNING,
  ERROR,
  CRITICAL
};

std::string LevelToString(Level level);
Level StringToLevel(std::string level, Level default_level=WARNING);

}

#endif  // TYRION_LEVEL_H_
