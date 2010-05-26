/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef TYRION_XMPP_SERVICE_MANAGER_H_
#define TYRION_XMPP_SERVICE_MANAGER_H_

#include <gloox/client.h>
#include <gloox/disco.h>
#include "xmpp_service.h"

namespace tyrion
{

  class XmppServiceManager : public gloox::IqHandler
  {
    public:
      XmppServiceManager(gloox::ClientBase* parent);
      virtual ~XmppServiceManager();

    protected:
      gloox::ClientBase* m_parent;
  };

}

#endif
