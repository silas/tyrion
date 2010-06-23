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

#include "node_xmpp_pump.h"

#include <txmpp/logging.h>
#include <txmpp/prexmppauthimpl.h>
#include "logging.h"
#include "node_xmpp_service_task.h"
#include "xmpp_presence_task.h"

namespace tyrion {

NodeXmppPump::NodeXmppPump(NodeXmppPumpNotify * notify) {
  state_ = txmpp::XmppEngine::STATE_NONE;
  notify_ = notify;
  client_ = new txmpp::XmppClient(this);  // deleted by TaskRunner
}

void NodeXmppPump::DoLogin(const txmpp::XmppClientSettings & xcs,
                       txmpp::XmppAsyncSocket* socket,
                       txmpp::PreXmppAuth* auth) {
  OnStateChange(txmpp::XmppEngine::STATE_START);

  if (!AllChildrenDone()) {
    client_->SignalStateChange.connect(this, &NodeXmppPump::OnStateChange);

    if (client_->Connect(xcs, "", socket, auth) != txmpp::XMPP_RETURN_OK) {
      TLOG(ERROR) << "Failed to connect.";
    }

    client_->Start();
  }
}

void NodeXmppPump::DoDisconnect() {
  if (!AllChildrenDone())
    client_->Disconnect();
  OnStateChange(txmpp::XmppEngine::STATE_CLOSED);
}

void NodeXmppPump::OnStateChange(txmpp::XmppEngine::State state) {
  if (state_ == state)
    return;

  int code = 0;

  switch(state) {
    case txmpp::XmppEngine::STATE_OPEN: {
      // Presence handler
      XmppPresenceTask *task_presence =
          new XmppPresenceTask(client_);  // owned by XmppClient
      task_presence->Start();
      // Service iq handler
      NodeXmppServiceTask *task_service =
          new NodeXmppServiceTask(client_);  // owned by XmppClient
      task_service->Start();
      }
      break;
    case txmpp::XmppEngine::STATE_START:
    case txmpp::XmppEngine::STATE_OPENING:
      break;
    case txmpp::XmppEngine::STATE_CLOSED:
      code = client_->GetError(NULL);
      break;
  }

  state_ = state;

  if (notify_ != NULL)
    notify_->OnStateChange(state, code);
}

void NodeXmppPump::WakeTasks() {
  txmpp::Thread::Current()->Post(this);
}

int64 NodeXmppPump::CurrentTime() {
  return (int64)txmpp::Time();
}

void NodeXmppPump::OnMessage(txmpp::Message *pmsg) {
  RunTasks();
}

txmpp::XmppReturnStatus NodeXmppPump::SendStanza(const txmpp::XmlElement *stanza) {
  if (!AllChildrenDone())
    return client_->SendStanza(stanza);

  return txmpp::XMPP_RETURN_BADSTATE;
}

}  // namespace tyrion
