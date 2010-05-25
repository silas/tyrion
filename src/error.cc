/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "error.h"

namespace tyrion
{

  std::string Error::Create(std::string code, std::string message)
  {
    return "Error: org.tyrion.error." + code
      + (!message.empty() ? ": " + message : message) + "\n";
  }

}
