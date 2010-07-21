/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_XMPP_PRESENCE_TASK_H_
#define _TYRION_XMPP_PRESENCE_TASK_H_

#include <txmpp/xmpptask.h>

namespace tyrion {

class XmppPresenceTask : public txmpp::XmppTask {
  public:
    explicit XmppPresenceTask(txmpp::TaskParent *parent);
    virtual ~XmppPresenceTask() {}
    virtual int ProcessStart();
    virtual int ProcessResponse();
    bool HandleStanza(const txmpp::XmlElement *stanza);
};

}  // namespace tyrion

#endif  // _TYRION_XMPP_PRESENCE_TASK_H_
