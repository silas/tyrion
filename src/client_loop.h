/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_CLIENT_LOOP_H_
#define _TYRION_CLIENT_LOOP_H_

#include "basic.h"
#include "client_settings.h"
#include "loop.h"

namespace tyrion {

class ClientEnvelope;
class ClientXmppPump;
class Loop;

class ClientLoop : public Loop {
  public:
    static const short MSG_REQUEST = 10;
    static const short MSG_RESPONSE = 11;
    typedef MessageDataType<ClientEnvelope> ServiceData;

    ClientLoop(pthread_t pthread);
    ~ClientLoop();

    void Request(ClientEnvelope* envelope);
    void Response(ClientEnvelope* envelope);

    inline ClientSettings* settings() { return settings_; }
    inline void set_settings(ClientSettings* settings) {
      Loop::set_settings(settings);
      settings_ = settings;
    }

  protected:
    void DoRequest(ServiceData* service);
    void DoResponse(ServiceData* service);

    void OnMessage(txmpp::Message* message);

    void SetupPump();

    int track_;
    ClientSettings* settings_;
    ClientXmppPump* pump_;
    DISALLOW_EVIL_CONSTRUCTORS(ClientLoop);
};

}  // namespace tyrion

#endif  // _TYRION_CLIENT_LOOP_H_
