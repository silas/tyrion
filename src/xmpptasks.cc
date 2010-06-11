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

#include <iostream>
#include <sstream>
#include <txmpp/constants.h>
#include <txmpp/logging.h>
#include <txmpp/xmppclient.h>
#include "xmpptasks.h"

namespace tyrion {

XmppTaskPresence::XmppTaskPresence(txmpp::TaskParent *parent)
    : txmpp::XmppTask(parent, txmpp::XmppEngine::HL_TYPE) {
}

XmppTaskPresence::~XmppTaskPresence() {
}

int XmppTaskPresence::ProcessStart() {

  txmpp::scoped_ptr<txmpp::XmlElement> presence(
      new txmpp::XmlElement(txmpp::QN_PRESENCE));
  SendStanza(presence.get());

  return STATE_RESPONSE;
}

int XmppTaskPresence::ProcessResponse() {

  const txmpp::XmlElement* stanza = NextStanza();
  if (stanza == NULL) {
    return STATE_BLOCKED;
  }

  std::string from = "Someone";

  if (stanza->HasAttr(txmpp::QN_FROM))
    from = stanza->Attr(txmpp::QN_FROM);

  std::cout << "Presence: " << from << std::endl;

  return STATE_RESPONSE;
}

bool XmppTaskPresence::HandleStanza(const txmpp::XmlElement *stanza) {

  if (stanza->Name() == txmpp::QN_PRESENCE) {
    QueueStanza(stanza);
    return true;
  }

  return false;
}

XmppTaskIq::XmppTaskIq(txmpp::TaskParent *parent)
    : txmpp::XmppTask(parent, txmpp::XmppEngine::HL_SINGLE) {
}

XmppTaskIq::~XmppTaskIq() {
}

int XmppTaskIq::ProcessStart() {

  set_task_id(GetClient()->NextId());

  txmpp::scoped_ptr<txmpp::XmlElement> privacy(
      MakeIq("get", txmpp::JID_EMPTY, task_id()));
  txmpp::XmlElement *query = new txmpp::XmlElement(txmpp::QN_PRIVACY_QUERY);
  privacy->AddElement(query);

  SendStanza(privacy.get());

  return STATE_RESPONSE;
}

int XmppTaskIq::ProcessResponse() {

  const txmpp::XmlElement* stanza = NextStanza();
  if (stanza == NULL) {
    return STATE_BLOCKED;
  }

  std::string from = "Someone";

  if (stanza->HasAttr(txmpp::QN_FROM))
    from = stanza->Attr(txmpp::QN_FROM);

  std::cout << "Iq: " << from << std::endl;

  return STATE_RESPONSE;
}

bool XmppTaskIq::HandleStanza(const txmpp::XmlElement *stanza) {

  if (MatchResponseIq(stanza, txmpp::JID_EMPTY, task_id())) {
    QueueStanza(stanza);
    return true;
  }

  return false;
}

}  // namespace tyrion
