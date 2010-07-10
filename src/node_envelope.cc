/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include "node_envelope.h"

#include <sstream>
#include <txmpp/constants.h>
#include "node_settings.h"
#include "utils.h"

namespace tyrion {

bool NodeEnvelope::Update(const txmpp::XmlElement *stanza) {
  if (stanza->Name() != txmpp::QN_IQ ||
      !stanza->HasAttr(txmpp::QN_FROM) ||
      !stanza->HasAttr(txmpp::QN_ID)) return false;

  jid_ = txmpp::Jid(stanza->Attr(txmpp::QN_FROM));
  id_ = stanza->Attr(txmpp::QN_ID);

  const txmpp::XmlElement *service = stanza->FirstWithNamespace(NS_SERVICE);

  if (service == NULL ||
      service->Name() != QN_SERVICE ||
      !service->HasAttr(txmpp::QN_TYPE)) return false;

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

bool NodeEnvelope::Check() {
  return NodeAcls::Instance()->GetBool(type_, jid_.BareJid().Str()) &&
      RealPath(Path()) != "";
}

const txmpp::XmlElement* NodeEnvelope::Response() {
  txmpp::XmlElement* iq = new txmpp::XmlElement(txmpp::QN_IQ);
  iq->SetAttr(txmpp::QN_TO, jid_.Str());
  iq->SetAttr(txmpp::QN_ID, id_);
  iq->SetAttr(txmpp::QN_TYPE, txmpp::STR_RESULT);

  txmpp::XmlElement* service = iq->FindOrAddNamedChild(QN_SERVICE);
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

std::string NodeEnvelope::Path() {
  return NodeSettings::Instance()->Get(SETTING_GENERAL,
      SETTING_SERVICE_PATH) + "/" + type_;
}

}  // namespace tyrion
