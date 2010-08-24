/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include "envelope.h"

#include <sstream>
#include <txmpp/constants.h>
#include <txmpp/xmlelement.h>
#include "loop.h"
#include "settings.h"
#include "utils.h"

namespace tyrion {

Envelope::Envelope(Loop* loop) :
  loop_(loop),
  jid_(txmpp::STR_EMPTY),
  id_(txmpp::STR_EMPTY),
  iq_id_(txmpp::STR_EMPTY),
  input_(txmpp::STR_EMPTY),
  output_(txmpp::STR_EMPTY),
  error_(txmpp::STR_EMPTY),
  type_(txmpp::STR_EMPTY),
  user_(txmpp::STR_EMPTY),
  group_(txmpp::STR_EMPTY),
  code_(0),
  timeout_(PROCESS_TIMEOUT),
  retry_(0) {
}

bool Envelope::Check() {
  return loop_->acls()->GetBool(type_, jid_.BareJid().Str()) &&
      RealPath(Path()) != "";
}

int Envelope::Retry() {
  if (retry_ <= 0) {
    retry_ = 1;
  } else if (retry_ < 32) {
    retry_ *= 2;
  }
  return retry_;
}

std::string Envelope::Path() {
  return loop_->settings()->Get(SETTING_GENERAL, SETTING_SERVICE_PATH) +
      "/" + type_;
}

const txmpp::XmlElement* Envelope::Response() {
  txmpp::XmlElement* iq = new txmpp::XmlElement(txmpp::QN_IQ);
  iq->SetAttr(txmpp::QN_TO, jid_.Str());
  iq->SetAttr(txmpp::QN_ID, iq_id_);
  iq->SetAttr(txmpp::QN_TYPE, txmpp::STR_RESULT);

  txmpp::XmlElement* service = iq->FindOrAddNamedChild(QN_SERVICE);
  service->SetAttr(txmpp::QN_ID, id_);
  service->SetAttr(txmpp::QN_TYPE, type_);

  std::ostringstream code_stream;
  code_stream << code_;
  service->SetAttr(QN_CODE, code_stream.str());

  txmpp::XmlElement* output = service->FindOrAddNamedChild(QN_OUTPUT);
  output->SetBodyText(output_);

  txmpp::XmlElement* error = service->FindOrAddNamedChild(QN_ERROR);
  error->SetBodyText(error_);

  return iq;
}

bool Envelope::Update(const txmpp::XmlElement *stanza) {
  if (stanza->Name() != txmpp::QN_IQ ||
      !stanza->HasAttr(txmpp::QN_FROM) ||
      !stanza->HasAttr(txmpp::QN_ID)) return false;

  jid_ = txmpp::Jid(stanza->Attr(txmpp::QN_FROM));
  iq_id_ = stanza->Attr(txmpp::QN_ID);

  const txmpp::XmlElement *service = stanza->FirstWithNamespace(NS_SERVICE);

  if (service == NULL ||
      service->Name() != QN_SERVICE ||
      !service->HasAttr(QN_SERVICE_ID) ||
      !service->HasAttr(txmpp::QN_TYPE)) return false;

  id_ = service->Attr(txmpp::QN_TYPE);
  type_ = service->Attr(txmpp::QN_TYPE);

  if (service->HasAttr(QN_USER))
    user_ = service->Attr(QN_USER);

  if (service->HasAttr(QN_GROUP))
    group_ = service->Attr(QN_GROUP);

  int timeout = 0;
  if (service->HasAttr(QN_TIMEOUT)) {
    std::istringstream timeout_stream(service->Attr(QN_TIMEOUT));
    timeout_stream >> timeout;
    set_timeout(timeout);
  }

  const txmpp::XmlElement *input = service->FirstNamed(QN_INPUT);

  if (input == NULL) return false;

  input_ = input->BodyText();

  return true;
}

}  // namespace tyrion
