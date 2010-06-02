/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef TYRION_NODE_XMPP_SERVICE_MANAGER_H_
#define TYRION_NODE_XMPP_SERVICE_MANAGER_H_

#include <pthread.h>
#include <gloox/client.h>
#include <gloox/disco.h>
#include "xmpp_service_manager.h"

namespace tyrion {
namespace node {

/**
 * Handle Tyrion service IQ.
 */
class XmppServiceManager : public tyrion::XmppServiceManager {
  public:
    XmppServiceManager(gloox::ClientBase* parent);
    virtual bool handleIq(const gloox::IQ& iq);
    static void *handleIqInThread(void *arg);
    virtual void handleIqID(const gloox::IQ& iq, int context) { (void)iq; }

  protected:
    std::string service_path_;
};

} }  // namespace tyrion::node

#endif  // TYRION_NODE_XMPP_SERVICE_MANAGER_H_
