/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef TYRION_CLIENT_XMPP_H_
#define TYRION_CLIENT_XMPP_H_

#include "client_service_queue.h"
#include "client_xmpp_service_manager.h"
#include "queue.h"
#include "xmpp.h"
#include "xmpp_service.h"

namespace tyrion {
namespace client {

class XmppServiceManager;

class Xmpp : public tyrion::Xmpp {
  public:
    // Setup XMPP client for CLI application with request and response
    // queues.
    Xmpp(ServiceQueue *request, ServiceQueue *response);

    virtual void SetupHandlers();
    virtual void DestroyHandlers();

    virtual void onConnect();

    ServiceQueue *request() { return request_; }
    ServiceQueue *response() { return response_; }

  private:
    ServiceQueue *request_;
    ServiceQueue *response_;
    XmppServiceManager *service_manager_;
};

} }  // namespace tyrion::client

#endif  // TYRION_CLIENT_XMPP_H_
