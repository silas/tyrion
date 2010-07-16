/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_CLIENT_XMPP_PUMP_H_
#define _TYRION_CLIENT_XMPP_PUMP_H_

#include "basic.h"
#include "xmpp_pump.h"

namespace tyrion {

class ClientLoop;

class ClientXmppPump : public XmppPump {
  public:
    ClientXmppPump(ClientLoop* loop);

    void DoOpen();
  private:
    ClientLoop* loop_;
    DISALLOW_EVIL_CONSTRUCTORS(ClientXmppPump);
};

}  // namespace tyrion

#endif  // _TYRION_CLIENT_XMPP_PUMP_H_
