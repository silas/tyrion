/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_NODE_XMPP_PUMP_H_
#define _TYRION_NODE_XMPP_PUMP_H_

#include "xmpp_pump.h"

namespace tyrion {

class NodeLoop;

class NodeXmppPump : public XmppPump {
  public:
    NodeXmppPump(XmppPumpNotify* notify) : XmppPump(notify) {}

    void DoOpen();
  private:
    NodeLoop* loop_;
};

}  // namespace tyrion

#endif  // _TYRION_NODE_XMPP_PUMP_H_
