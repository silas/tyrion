/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "node_xmpp_service_manager.h"

#include "node_acl.h"
#include "node_service_handler.h"
#include "setting.h"
#include "tyrion.h"
#include "utils.h"

namespace tyrion {
namespace node {

XmppServiceManager::XmppServiceManager(gloox::ClientBase* parent) :
    tyrion::XmppServiceManager(parent) {
  service_path_ = utils::RealPath(
    Setting::Instance()->Get("general", "service", SERVICE_PATH));
}

void *XmppServiceManager::handleIqInThread(void *arg) {
  ServiceHandler *handler=(ServiceHandler*)arg;
  handler->Run();
  delete(handler);
  pthread_exit(NULL);
}

bool XmppServiceManager::handleIq(const gloox::IQ& iq) {
  const tyrion::XmppService* xs =
      iq.findExtension<tyrion::XmppService>(tyrion::ExtXmppService);

  if (xs) {
    bool bare = Acl::Instance()->GetBool(xs->type(), iq.from().bare());
    bool full = Acl::Instance()->GetBool(xs->type(), iq.from().full());
    bool has_full = Acl::Instance()->Has(xs->type(), iq.from().full());

    if (full || (bare && !has_full)) {
      ServiceHandler *serviceHandler = new ServiceHandler(
          (gloox::ClientBase *)parent_,
          iq.from(),
          iq.id(),
          xs->type(),
          xs->input(),
          service_path_ + "/" + xs->type()
      );
      serviceHandler->set_timeout(xs->timeout());
      serviceHandler->set_user(xs->user());
      serviceHandler->set_group(xs->group());

      return utils::CreateThread(XmppServiceManager::handleIqInThread,
                                 (void *)serviceHandler);
    }
  }
  return false;
}

} } // namespace tyrion::node
