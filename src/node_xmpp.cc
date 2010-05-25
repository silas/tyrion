/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "node_xmpp.h"

#include <unistd.h>

#include <string>

#include "logging.h"
#include "node_service_handler.h"
#include "setting.h"
#include "tyrion.h"

namespace tyrion
{
  namespace node
  {

    void Xmpp::SetupHandlers()
    {
      service_manager_ = new XmppServiceManager(client_);
    }

    void Xmpp::DestroyHandlers()
    {
      delete(service_manager_);
    }

  }
}
