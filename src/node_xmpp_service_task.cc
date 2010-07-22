/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include "node_xmpp_service_task.h"

#include <txmpp/constants.h>
#include <txmpp/logging.h>
#include <txmpp/xmppclient.h>
#include "common.h"
#include "node_envelope.h"
#include "node_loop.h"

namespace tyrion {

NodeXmppServiceTask::NodeXmppServiceTask(NodeLoop* loop,
                                         txmpp::TaskParent *parent)
    : txmpp::XmppTask(parent, txmpp::XmppEngine::HL_TYPE), loop_(loop) {
}

int NodeXmppServiceTask::ProcessStart() {
  return STATE_RESPONSE;
}

int NodeXmppServiceTask::ProcessResponse() {

  const txmpp::XmlElement* stanza = NextStanza();

  if (stanza == NULL)
    return STATE_BLOCKED;

  NodeEnvelope *envelope = new NodeEnvelope(loop_);
  envelope->Update(stanza);
  loop_->Request(envelope);

  return STATE_RESPONSE;
}

bool NodeXmppServiceTask::HandleStanza(const txmpp::XmlElement *stanza) {

  NodeEnvelope envelope(loop_);

  if (envelope.Update(stanza) && envelope.Check()) {
    QueueStanza(stanza);
    return true;
  }

  return false;
}

}  // namespace tyrion
