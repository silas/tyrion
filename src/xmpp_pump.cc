/*
 * Copyright (c) 2010, Silas Sewell
 * All rights reserved.
 *
 * This file is subject to the MIT License (see the LICENSE file).
 */

#include "xmpp_pump.h"

#include <txmpp/prexmppauthimpl.h>
#include "loop.h"
#include "xmpp_service_task.h"
#include "xmpp_presence_task.h"

namespace tyrion {

XmppPump::XmppPump(Loop* loop) :
    client_(NULL),
    state_(txmpp::XmppEngine::STATE_NONE),
    loop_(loop) {
  client_ = new txmpp::XmppClient(this);  // deleted by TaskRunner
}

void XmppPump::DoOpen() {
  // Presence handler
  XmppPresenceTask *task_presence =
      new XmppPresenceTask(client_);  // owned by XmppClient
  task_presence->Start();

  // Service iq handler
  XmppServiceTask *task_service =
      new XmppServiceTask(loop_, client_);  // owned by XmppClient
  task_service->Start();
}

void XmppPump::DoLogin(const txmpp::XmppClientSettings & xcs,
                       txmpp::XmppAsyncSocket* socket,
                       txmpp::PreXmppAuth* auth) {
  OnStateChange(txmpp::XmppEngine::STATE_START);

  if (!AllChildrenDone()) {
    client_->SignalStateChange.connect(this, &XmppPump::OnStateChange);

    if (client_->Connect(xcs, "", socket, auth) != txmpp::XMPP_RETURN_OK) {
      TLOG(ERROR) << "Failed to connect.";
      // TODO(silas): figure out if I should delete socket and auth
    }

    client_->Start();
  }
}

void XmppPump::DoDisconnect() {
  if (!AllChildrenDone())
    client_->Disconnect();
  OnStateChange(txmpp::XmppEngine::STATE_CLOSED);
}

void XmppPump::OnStateChange(txmpp::XmppEngine::State state) {
  if (state_ == state)
    return;

  int code = 0;

  switch(state) {
    case txmpp::XmppEngine::STATE_OPEN:
      DoOpen();
      break;
    case txmpp::XmppEngine::STATE_START:
    case txmpp::XmppEngine::STATE_OPENING:
    case txmpp::XmppEngine::STATE_NONE:
      break;
    case txmpp::XmppEngine::STATE_CLOSED:
      code = client_->GetError(NULL);
      break;
  }

  state_ = state;

  loop_->OnStateChange(state, code);
}

void XmppPump::WakeTasks() {
  txmpp::Thread::Current()->Post(this);
}

int64 XmppPump::CurrentTime() {
  return (int64)txmpp::Time();
}

void XmppPump::OnMessage(txmpp::Message *message) {
  RunTasks();
}

txmpp::XmppReturnStatus XmppPump::SendStanza(const txmpp::XmlElement *stanza) {
  if (!AllChildrenDone())
    return client_->SendStanza(stanza);

  return txmpp::XMPP_RETURN_BADSTATE;
}

}  // namespace tyrion
