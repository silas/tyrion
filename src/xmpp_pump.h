/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_XMPP_PUMP_H_
#define _TYRION_XMPP_PUMP_H_

#include <txmpp/messagequeue.h>
#include <txmpp/taskrunner.h>
#include <txmpp/thread.h>
#include <txmpp/time.h>
#include <txmpp/xmppclient.h>
#include <txmpp/xmppengine.h>
#include <txmpp/xmpptask.h>

namespace tyrion {

class XmppPumpNotify {
  public:
    virtual ~XmppPumpNotify() {}
    virtual void OnStateChange(txmpp::XmppEngine::State state,
                               int code = 0) = 0;
};

class XmppPump : public txmpp::MessageHandler, public txmpp::TaskRunner {
  public:
    XmppPump(XmppPumpNotify* notify);

    txmpp::XmppReturnStatus SendStanza(const txmpp::XmlElement *stanza);
    int64 CurrentTime();

    void DoLogin(const txmpp::XmppClientSettings & xcs,
                 txmpp::XmppAsyncSocket* socket,
                 txmpp::PreXmppAuth* auth);
    virtual void DoOpen() {}
    void DoDisconnect();
    void WakeTasks();

    void OnStateChange(txmpp::XmppEngine::State state);
    void OnMessage(txmpp::Message *pmsg);

    inline txmpp::XmppClient *client() { return client_; }

  protected:
    txmpp::XmppClient *client_;
    txmpp::XmppEngine::State state_;
    XmppPumpNotify *notify_;
};

}  // namespace tyrion

#endif  // _TYRION_XMPP_PUMP_H_
