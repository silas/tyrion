/*
 * Tyrion
 * Copyright 2010, Silas Sewell
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _TYRION_NODE_XMPP_PUMP_H_
#define _TYRION_NODE_XMPP_PUMP_H_

#include <pthread.h>
#include <txmpp/messagequeue.h>
#include <txmpp/taskrunner.h>
#include <txmpp/thread.h>
#include <txmpp/time.h>
#include <txmpp/xmppclient.h>
#include <txmpp/xmppengine.h>
#include <txmpp/xmpptask.h>

namespace tyrion {

class NodeXmppPumpNotify {
  public:
    virtual ~NodeXmppPumpNotify() {}
    virtual void OnStateChange(txmpp::XmppEngine::State state,
                               int code = 0) = 0;
};

class NodeXmppPump : public txmpp::MessageHandler, public txmpp::TaskRunner {
  public:
    NodeXmppPump(NodeXmppPumpNotify * notify = NULL);

    void Stop();

    txmpp::XmppClient *client() { return client_; }
    txmpp::XmppReturnStatus SendStanza(const txmpp::XmlElement *stanza);
    int64 CurrentTime();

    void DoLogin(const txmpp::XmppClientSettings & xcs,
                 txmpp::XmppAsyncSocket* socket,
                 txmpp::PreXmppAuth* auth);
    void DoDisconnect();
    void WakeTasks();

    void OnStateChange(txmpp::XmppEngine::State state);
    void OnMessage(txmpp::Message *pmsg);

  private:
    txmpp::XmppClient *client_;
    txmpp::XmppEngine::State state_;
    NodeXmppPumpNotify *notify_;
    pthread_mutex_t mutex_;
};

}  // namespace tyrion

#endif  // _TYRION_NODE_XMPP_PUMP_H_
