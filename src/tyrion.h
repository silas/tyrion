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

#include <string>

namespace tyrion
{

  const std::string ACL_PATH         = "/etc/tyrion/acl.conf";
  const std::string CONFIG_PATH      = "/etc/tyrion/node.conf";
  const std::string LOG_PATH         = "/var/log/tyrion.log";
  const std::string SERVICE_PATH     = "/var/lib/tyrion/service";
  const int         SERVICE_TIMEOUT  = 60;
  const std::string VERSION          = "0.0.1";
  const std::string XMLNS_IQ_SERVICE = "http://tyrion.org/protocol/1.0/service";

}

#endif
