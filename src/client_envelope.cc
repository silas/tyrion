/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include "client_envelope.h"

#include <sstream>
#include <txmpp/constants.h>
#include "utils.h"
#include "logging.h"

namespace tyrion {

bool ClientEnvelope::Update(const txmpp::XmlElement *stanza) {

  if (stanza->Name() != txmpp::QN_IQ ||
      !stanza->HasAttr(txmpp::QN_FROM) ||
      !stanza->HasAttr(txmpp::QN_TYPE)) return false;

  if (stanza->Attr(txmpp::QN_TYPE) == "error") {
    code_ = -1;
    error_ += CreateError("xmpp", stanza->Str());
  }

  if (!stanza->HasAttr(txmpp::QN_ID)) return false;

  const txmpp::XmlElement *service = stanza->FirstWithNamespace(NS_SERVICE);

  if (service == NULL ||
      service->Name() != QN_SERVICE ||
      !service->HasAttr(txmpp::QN_CODE) ||
      !service->HasAttr(txmpp::QN_TYPE)) return false;

  std::istringstream code_stream(service->Attr(QN_CODE));
  code_stream >> code_;

  type_ = service->Attr(txmpp::QN_TYPE);

  if (service->HasAttr(QN_USER))
    user_ = service->Attr(QN_USER);

  if (service->HasAttr(QN_GROUP))
    group_ = service->Attr(QN_GROUP);

  const txmpp::XmlElement *output = service->FirstNamed(QN_OUTPUT);

  if (output == NULL) return false;
  output_ += output->BodyText();

  const txmpp::XmlElement *error = service->FirstNamed(QN_ERROR);

  if (error == NULL) return false;
  error_ += error->BodyText();

  return true;
}

txmpp::XmlElement* ClientEnvelope::Request() {
  txmpp::XmlElement* iq = new txmpp::XmlElement(txmpp::QN_IQ);
  iq->SetAttr(txmpp::QN_TO, jid_.Str());
  iq->SetAttr(txmpp::QN_ID, id_);
  iq->SetAttr(txmpp::QN_TYPE, txmpp::STR_SET);

  txmpp::XmlElement* service = iq->FindOrAddNamedChild(QN_SERVICE);
  service->SetAttr(txmpp::QN_TYPE, type_);

  std::ostringstream timeout_stream;
  timeout_stream << timeout_;
  service->SetAttr(QN_TIMEOUT, timeout_stream.str());

  txmpp::XmlElement* input = service->FindOrAddNamedChild(QN_INPUT);
  input->SetBodyText(input_);

  return iq;
}

}  // namespace tyrion
