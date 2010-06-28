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

  jid_ = txmpp::Jid(stanza->Attr(txmpp::QN_FROM));
  id_ = stanza->Attr(txmpp::QN_ID);

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

const txmpp::XmlElement* ClientEnvelope::Request() {
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

};  // namespace tyrion
