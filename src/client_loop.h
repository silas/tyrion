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

class ClientLoop : public Loop {
  public:
    static const short MSG_REQUEST = 10;
    static const short MSG_RESPONSE = 11;
    typedef MessageDataType<ClientEnvelope> ServiceData;

    ClientLoop(pthread_t pthread);

    void Request(ClientEnvelope* envelope);
    void Response(ClientEnvelope* envelope);

    inline ClientSettings* settings() { return settings_; }

  protected:
    void DoRequest(ServiceData* service);
    void DoResponse(ServiceData* service);

    void OnMessage(txmpp::Message* message);

    int track_;
    ClientSettings* settings_;
    ClientXmppPump* pump_;
};

}  // namespace tyrion

#endif  // _TYRION_CLIENT_LOOP_H_
