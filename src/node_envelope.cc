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

#include "node_envelope.h"

#include <sstream>
#include <txmpp/constants.h>
#include "node_settings.h"
#include "utils.h"

namespace tyrion {

NodeServiceEnvelope::NodeServiceEnvelope() {
  code_ = 0;
  timeout_ = PROCESS_TIMEOUT;
  valid_ = false;
}

NodeServiceEnvelope::NodeServiceEnvelope(const txmpp::XmlElement *stanza) {
  code_ = 0;
  timeout_ = PROCESS_TIMEOUT;
  valid_ = false;

  if (stanza->Name() != txmpp::QN_IQ ||
      !stanza->HasAttr(txmpp::QN_FROM) ||
      !stanza->HasAttr(txmpp::QN_ID)) return;

  jid_ = txmpp::Jid(stanza->Attr(txmpp::QN_FROM));
  id_ = stanza->Attr(txmpp::QN_ID);

  const txmpp::XmlElement *service = stanza->FirstWithNamespace(NS_SERVICE);

  if (service == NULL ||
      service->Name() != QN_SERVICE ||
      !service->HasAttr(txmpp::QN_TYPE) ||
      !service->HasAttr(txmpp::QN_XMLNS) ||
      service->Attr(txmpp::QN_XMLNS) != NS_SERVICE) return;

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

  if (input == NULL) return;

  input_ = input->BodyText();

  valid_ = true;
}

NodeServiceEnvelope::~NodeServiceEnvelope() {
}

bool NodeServiceEnvelope::ValidRequest() {
  return valid_ && NodeAcls::Instance()->GetBool(type_, jid_.BareJid().Str()) && RealPath(Path()) != "";
}

const txmpp::XmlElement* NodeServiceEnvelope::Response() {
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

std::string NodeServiceEnvelope::Path() {
  return NodeSettings::Instance()->Get(STR_GENERAL, STR_SERVICE_PATH) \
      + "/" + type_;
}

};  // namespace tyrion
