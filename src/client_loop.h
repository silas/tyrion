/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_CLIENT_LOOP_H_
#define _TYRION_CLIENT_LOOP_H_

#include "loop.h"
#include "client_envelope.h"
#include "client_settings.h"
#include "client_xmpp_pump.h"

namespace tyrion {

typedef Loop<ClientEnvelope, ClientSettings, ClientXmppPump> BaseLoop;

class ClientLoop : public BaseLoop {
  public:
    ClientLoop();

  protected:
    void DoRestart(bool delay = true);
    void DoRequest(ServiceData* service);
    void DoResponse(ServiceData* service);

    int track;
};

}  // namespace tyrion

#endif  // _TYRION_CLIENT_LOOP_H_
