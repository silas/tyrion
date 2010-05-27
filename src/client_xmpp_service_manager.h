/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef TYRION_CLIENT_XMPP_SERVICE_MANAGER_H_
#define TYRION_CLIENT_XMPP_SERVICE_MANAGER_H_

#include <pthread.h>
#include <gloox/client.h>
#include <gloox/disco.h>
#include "client_xmpp.h"
#include "xmpp_service_manager.h"

namespace tyrion {
namespace client {

    class Xmpp;

    class XmppServiceManager : public tyrion::XmppServiceManager {
      public:
        XmppServiceManager(Xmpp *xmpp);

        virtual bool handleIq(const gloox::IQ& iq) { return true; }
        virtual void handleIqID(const gloox::IQ& iq, int context);

      private:
        Xmpp* xmpp_;
    };

} }  // namespace tyrion::client

#endif  // TYRION_CLIENT_XMPP_SERVICE_MANAGER_H_
