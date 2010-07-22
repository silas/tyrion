/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_CLIENT_XMPP_SERVICE_TASK_H_
#define _TYRION_CLIENT_XMPP_SERVICE_TASK_H_

#include <txmpp/xmpptask.h>
#include "common.h"

namespace txmpp {

class XmppParent;

}  // namespace txmpp

namespace tyrion {

class ClientEnvelope;
class ClientLoop;

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
    DISALLOW_EVIL_CONSTRUCTORS(ClientXmppServiceTask);
};

}  // namespace tyrion

#endif  // _TYRION_CLIENT_XMPP_SERVICE_TASK_H_
