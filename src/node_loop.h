/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_NODE_LOOP_H_
#define _TYRION_NODE_LOOP_H_

#include "basic.h"
#include "loop.h"
#include "node_service_handler.h"
#include "node_settings.h"

namespace tyrion {

class NodeXmppPump;

class NodeLoop : public Loop {
  public:
    static const short MSG_REQUEST = 10;
    static const short MSG_RESPONSE = 11;
    static const short MSG_RESTART = 12;
    static const short MSG_SET_RECONNECT = 13;
    typedef MessageDataType<NodeEnvelope> ServiceData;
    typedef txmpp::TypedMessageData<bool> ReconnectData;

    NodeLoop(pthread_t pthread);
    ~NodeLoop();

    void SetReconnect(bool reconnect);
    void Restart();
    void Request(NodeEnvelope* envelope);
    void Response(NodeEnvelope* envelope);

    inline NodeSettings* settings() { return settings_; }
    inline void set_settings(NodeSettings* settings) {
      Loop::set_settings(settings);
      settings_ = settings;
    }

    inline NodeAcls* acls() { return acls_; }
    inline void set_acls(NodeAcls* acls) { acls_ = acls; }

    inline NodeServiceHandler* service_handler() { return service_handler_; }
    inline void set_service_handler(NodeServiceHandler* service_handler) {
      service_handler_ = service_handler;
      service_handler_->set_loop(this);
    }

  protected:
    void DoSetReconnect(ReconnectData* reconnect);
    void DoRestart();
    void DoRequest(ServiceData* service);
    void DoResponse(ServiceData* service);

    void OnMessage(txmpp::Message* message);

    void SetupPump();

    int track_;
    NodeSettings* settings_;
    NodeAcls* acls_;
    NodeXmppPump* pump_;
    NodeServiceHandler* service_handler_;
    bool reconnect_;
    DISALLOW_EVIL_CONSTRUCTORS(NodeLoop);
};

}  // namespace tyrion

#endif  // _TYRION_NODE_LOOP_H_
