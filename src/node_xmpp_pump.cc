/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include "node_xmpp_pump.h"

#include "node_xmpp_service_task.h"
#include "xmpp_presence_task.h"

namespace tyrion {

void NodeXmppPump::DoOpen() {
  // Presence handler
  XmppPresenceTask *task_presence =
      new XmppPresenceTask(client_);  // owned by XmppClient
  task_presence->Start();

  // Service iq handler
  NodeXmppServiceTask *task_service =
      new NodeXmppServiceTask(client_);  // owned by XmppClient
  task_service->Start();
}

}  // namespace tyrion
