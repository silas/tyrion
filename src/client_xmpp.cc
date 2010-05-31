/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "client_xmpp.h"

#include <unistd.h>
#include <string>
#include "logging.h"
#include "setting.h"
#include "tyrion.h"

namespace tyrion {
namespace client {

Xmpp::Xmpp(ServiceQueue *request, ServiceQueue *response) {
  request_ = request;
  response_ = response;
}

void Xmpp::SetupHandlers() {
  service_manager_ = new XmppServiceManager(this);
}

void Xmpp::DestroyHandlers() {
  delete(service_manager_);
}

void Xmpp::onConnect() {
  tyrion::Xmpp::onConnect();

  ServiceQueueItem item = request()->Pop();
  XmppService *service = new XmppService(item.service());

  gloox::IQ re(gloox::IQ::Set, item.jid(), item.id());
  re.addExtension(service);
  client_->send(re, (gloox::IqHandler *)service_manager_,
                item.notification(), false);
}

} } // namespace tyrion::client
