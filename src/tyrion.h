/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef TYRION_TYRION_H_
#define TYRION_TYRION_H_

#define OPTION(s, l) (strcmp(option, s) == 0 || strcmp(option, l) == 0)

#include <string>

namespace tyrion {

const int         NODE_RECONNECT   = 10;
const int         SERVICE_TIMEOUT  = 60;
const std::string VERSION          = "0.0.1";
const std::string XMLNS_IQ_SERVICE = "http://tyrion.org/protocol/1.0/service";

}  // namespace tyrion

#endif  // TYRION_TYRION_H_
