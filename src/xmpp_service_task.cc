/*
 * Copyright (c) 2010, Silas Sewell
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include "xmpp_service_task.h"

#include <txmpp/constants.h>
#include <txmpp/logging.h>
#include <txmpp/xmppclient.h>
#include "common.h"
#include "envelope.h"
#include "loop.h"

namespace tyrion {

XmppServiceTask::XmppServiceTask(Loop* loop,
                                 txmpp::TaskParent *parent)
    : txmpp::XmppTask(parent, txmpp::XmppEngine::HL_TYPE), loop_(loop) {
}

int XmppServiceTask::ProcessStart() {
  return STATE_RESPONSE;
}

int XmppServiceTask::ProcessResponse() {

  const txmpp::XmlElement* stanza = NextStanza();

  if (stanza == NULL)
    return STATE_BLOCKED;

  Envelope *envelope = new Envelope(loop_);
  envelope->Update(stanza);
  loop_->Request(envelope);

  return STATE_RESPONSE;
}

bool XmppServiceTask::HandleStanza(const txmpp::XmlElement *stanza) {

  Envelope envelope(loop_);

  if (envelope.Update(stanza) && envelope.Check()) {
    QueueStanza(stanza);
    return true;
  }

  return false;
}

}  // namespace tyrion
