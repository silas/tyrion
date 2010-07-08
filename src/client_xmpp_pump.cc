/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include "client_xmpp_pump.h"

#include "xmpp_presence_task.h"

namespace tyrion {

void ClientXmppPump::DoOpen() {
  // Presence handler
  XmppPresenceTask *task_presence =
      new XmppPresenceTask(client_);  // owned by XmppClient
  task_presence->Start();
}

}  // namespace tyrion
