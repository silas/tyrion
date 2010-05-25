/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef TYRION_UTILS_H_
#define TYRION_UTILS_H_

#include <string>

namespace tyrion
{
  namespace utils
  {

    std::string RealPath(std::string path);
    bool CreateThread(void *func(void*), void *arg);

  }
}

#endif
