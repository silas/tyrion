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

#include "client_xmpp_service_task.h"

#include <txmpp/constants.h>
#include <txmpp/logging.h>
#include <txmpp/xmppclient.h>
#include "constants.h"
#include "logging.h"
#include "client_loop.h"

namespace tyrion {

ClientXmppServiceTask::ClientXmppServiceTask(txmpp::TaskParent *parent,
                                             ClientEnvelope* envelope)
    : txmpp::XmppTask(parent, txmpp::XmppEngine::HL_SINGLE) {
  envelope_ = envelope;
  set_task_id(GetClient()->NextId());
  envelope_->set_id(task_id());
}

ClientXmppServiceTask::~ClientXmppServiceTask() {
}

int ClientXmppServiceTask::ProcessStart() {
  SendStanza(envelope_->Request());
  return STATE_RESPONSE;
}

int ClientXmppServiceTask::ProcessResponse() {
  const txmpp::XmlElement* stanza = NextStanza();

  if (stanza == NULL)
    return STATE_BLOCKED;

  envelope_->Update(stanza);
  tyrion::ClientLoop::Instance()->Response(envelope_);

  return STATE_RESPONSE;
}

bool ClientXmppServiceTask::HandleStanza(const txmpp::XmlElement *stanza) {

  if (MatchResponseIq(stanza, envelope_->jid(), task_id())) {
    QueueStanza(stanza);
    return true;
  }

  return false;
}

}  // namespace tyrion
