/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
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
  envelope_->set_id(GetClient()->NextId());
}

ClientXmppServiceTask::~ClientXmppServiceTask() {
  delete envelope_;
}

int ClientXmppServiceTask::ProcessStart() {
  txmpp::XmlElement* message = envelope_->Request();
  SendStanza(message);
  delete message;
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
  if (IsValid(stanza)) {
    QueueStanza(stanza);
    return true;
  }

  return false;
}

bool ClientXmppServiceTask::IsValid(const txmpp::XmlElement *stanza) {
  if (stanza->Name() != txmpp::QN_IQ ||
      !stanza->HasAttr(txmpp::QN_FROM) ||
      !stanza->HasAttr(txmpp::QN_TYPE)) return false;

  if (stanza->Attr(txmpp::QN_TYPE) != "result" &&
      stanza->Attr(txmpp::QN_TYPE) != "error") return false;

  if (!stanza->HasAttr(txmpp::QN_ID)) return false;

  const txmpp::XmlElement *service = stanza->FirstWithNamespace(NS_SERVICE);

  if (service == NULL ||
      service->Name() != QN_SERVICE ||
      !service->HasAttr(txmpp::QN_CODE) ||
      !service->HasAttr(txmpp::QN_TYPE)) return false;

  return MatchResponseIq(stanza, envelope_->jid(), envelope_->id());
}

}  // namespace tyrion
