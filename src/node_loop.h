/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_NODE_LOOP_H_
#define _TYRION_NODE_LOOP_H_

#include "loop.h"
#include "node_envelope.h"
#include "node_settings.h"
#include "node_xmpp_pump.h"

namespace tyrion {

class NodeServiceHandler;

class NodeLoop : public Loop {
  public:
    static const short MSG_REQUEST = 10;
    static const short MSG_RESPONSE = 11;
    static const short MSG_RESTART = 12;
    static const short MSG_RELOAD = 13;
    typedef MessageDataType<NodeEnvelope> ServiceData;

    NodeLoop(pthread_t pthread, NodeServiceHandler* service_handler);

    void Reload();
    void Restart();
    void Request(NodeEnvelope* envelope);
    void Response(NodeEnvelope* envelope);

    inline NodeSettings* settings() { return settings_; }
    inline NodeAcls* acls() { return acls_; }

  protected:
    void DoReload();
    void DoRestart(bool delay = true);
    void DoRequest(ServiceData* service);
    void DoResponse(ServiceData* service);

    void OnMessage(txmpp::Message* message);

    int track_;
    NodeSettings* settings_;
    NodeAcls* acls_;
    NodeXmppPump* pump_;
    NodeServiceHandler* service_handler_;
};

}  // namespace tyrion

#endif  // _TYRION_NODE_LOOP_H_
