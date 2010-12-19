/*
 * Copyright (c) 2010, Silas Sewell
 * All rights reserved.
 *
 * This file is subject to the MIT License (see the LICENSE file).
 */

#ifndef _TYRION_XMPP_PUMP_H_
#define _TYRION_XMPP_PUMP_H_

#include <txmpp/taskrunner.h>
#include <txmpp/thread.h>
#include <txmpp/xmppclient.h>
#include <txmpp/xmpptask.h>
#include "common.h"

namespace tyrion {

class Loop;

class XmppPump : public txmpp::MessageHandler, public txmpp::TaskRunner {
  public:
    XmppPump(Loop* loop);

    txmpp::XmppReturnStatus SendStanza(const txmpp::XmlElement *stanza);
    int64 CurrentTime();

    void DoLogin(const txmpp::XmppClientSettings & xcs,
                 txmpp::XmppAsyncSocket* socket,
                 txmpp::PreXmppAuth* auth);
    void DoOpen();
    void DoDisconnect();
    void WakeTasks();

    void OnStateChange(txmpp::XmppEngine::State state);
    void OnMessage(txmpp::Message *message);

    inline txmpp::XmppClient *client() { return client_; }

  protected:
    txmpp::XmppClient *client_;
    txmpp::XmppEngine::State state_;
    Loop *loop_;
    DISALLOW_EVIL_CONSTRUCTORS(XmppPump);
};

}  // namespace tyrion

#endif  // _TYRION_XMPP_PUMP_H_
