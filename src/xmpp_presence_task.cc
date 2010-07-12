/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include "xmpp_presence_task.h"

#include <txmpp/constants.h>
#include <txmpp/logging.h>
#include <txmpp/xmppclient.h>
#include "constants.h"
#include "logging.h"

namespace tyrion {

XmppPresenceTask::XmppPresenceTask(txmpp::TaskParent *parent)
    : txmpp::XmppTask(parent, txmpp::XmppEngine::HL_TYPE) {
}

int XmppPresenceTask::ProcessStart() {
  txmpp::scoped_ptr<txmpp::XmlElement> presence(
      new txmpp::XmlElement(txmpp::QN_PRESENCE));

  SendStanza(presence.get());

  return STATE_RESPONSE;
}

int XmppPresenceTask::ProcessResponse() {
  const txmpp::XmlElement* stanza = NextStanza();

  if (stanza == NULL)
    return STATE_BLOCKED;

  return STATE_RESPONSE;
}

bool XmppPresenceTask::HandleStanza(const txmpp::XmlElement *stanza) {

  if (stanza->Name() == txmpp::QN_PRESENCE) {
    QueueStanza(stanza);
    return true;
  }

  return false;
}

}  // namespace tyrion
