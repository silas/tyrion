/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_CLIENT_XMPP_PUMP_H_
#define _TYRION_CLIENT_XMPP_PUMP_H_

#include "xmpp_pump.h"

namespace tyrion {

class ClientXmppPump : public XmppPump {
  public:
    ClientXmppPump(XmppPumpNotify* notify) : XmppPump(notify) {}

    void DoOpen();
};

}  // namespace tyrion

#endif  // _TYRION_CLIENT_XMPP_PUMP_H_
