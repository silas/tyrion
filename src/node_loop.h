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

#ifndef _TYRION_NODE_LOOP_H_
#define _TYRION_NODE_LOOP_H_

#include <iostream>
#include <txmpp/thread.h>
#include <txmpp/xmppclientsettings.h>
#include "constants.h"
#include "node_envelope.h"
#include "node_xmpp_pump.h"

namespace tyrion {

class NodeLoop : public txmpp::Thread, XmppPumpNotify,
                        txmpp::MessageHandler,
                 public txmpp::has_slots<> {
  public:
    enum Message {
      MSG_LOGIN = 1,
      MSG_DISCONNECT,
      MSG_RESTART,
      MSG_RELOAD,
      MSG_SHUTDOWN,
      MSG_REQUEST,
      MSG_RESPONSE
    };
    enum State {
      NONE = 0,
      RUNNING,
      RESTARTING,
      ERROR
    };
    typedef txmpp::TypedMessageData<NodeServiceEnvelope*> ServiceData;

    static NodeLoop* Instance();

    ~NodeLoop();

    void Login();
    void Restart();
    void Reload();
    void Disconnect();

    void Request(NodeServiceEnvelope* envelope);
    void Response(NodeServiceEnvelope* envelope);

    void ProcessMessages(int cms);

    inline txmpp::XmppClient* client() { return pump_->client(); }
    inline State state() { return state_; }

  private:
    NodeLoop();

    void DoLogin();
    void DoDisconnect();
    void DoRestart(int delay = RECONNECT_TIMEOUT);
    void DoReload();
    void DoShutdown();

    void DoRequest(ServiceData* service);
    static void *DoRequestInThread(void *arg);
    void DoResponse(ServiceData* service);

    void OnMessage(txmpp::Message* pmsg);
    void OnSocketClose(int code);
    void OnStateChange(txmpp::XmppEngine::State state, int code = 0);

    static NodeLoop* instance_;
    NodeXmppPump* pump_;
    State state_;
};

}  // namespace tyrion

#endif  // _TYRION_XMPPTHREAD_H_
