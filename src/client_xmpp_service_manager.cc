/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "client_xmpp_service_manager.h"

#include "setting.h"
#include "tyrion.h"
#include "utils.h"

namespace tyrion
{
  namespace client
  {

    XmppServiceManager::XmppServiceManager(Xmpp *xmpp)
      : tyrion::XmppServiceManager(xmpp->GetClient()), xmpp_(xmpp)
    {}

    bool XmppServiceManager::handleIq(const gloox::IQ& iq)
    {
      return true;
    }

    void XmppServiceManager::handleIqID(const gloox::IQ& iq, int context)
    {
      const tyrion::XmppService* service = iq.findExtension<tyrion::XmppService>(tyrion::ExtXmppService);

      ServiceQueueItem item(iq.from().full(), *service, iq.id());
      xmpp_->response->Push(item);

      if (context == tyrion::client::Disconnect)
        xmpp_->Stop();
    }

  }
}
