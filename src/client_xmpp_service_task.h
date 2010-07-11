/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_CLIENT_XMPP_SERVICE_TASK_H_
#define _TYRION_CLIENT_XMPP_SERVICE_TASK_H_

#include <txmpp/taskparent.h>
#include <txmpp/xmpptask.h>
#include "client_envelope.h"
#include "client_loop.h"

namespace tyrion {

class ClientXmppServiceTask : public txmpp::XmppTask {
  public:
    ClientXmppServiceTask(ClientLoop* loop, txmpp::TaskParent *parent,
                          ClientEnvelope* envelope);
    virtual ~ClientXmppServiceTask();
    virtual int ProcessStart();
    virtual int ProcessResponse();
    bool HandleStanza(const txmpp::XmlElement *stanza);
    bool IsValid(const txmpp::XmlElement *stanza);

  private:
    ClientLoop* loop_;
    ClientEnvelope* envelope_;
};

}  // namespace tyrion

#endif  // _TYRION_CLIENT_XMPP_SERVICE_TASK_H_
