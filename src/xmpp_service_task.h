/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_XMPP_SERVICE_TASK_H_
#define _TYRION_XMPP_SERVICE_TASK_H_

#include <txmpp/xmpptask.h>

namespace tyrion {

class Loop;

class XmppServiceTask : public txmpp::XmppTask {
  public:
    explicit XmppServiceTask(Loop* loop, txmpp::TaskParent *parent);
    ~XmppServiceTask() {}
    int ProcessStart();
    int ProcessResponse();
    bool HandleStanza(const txmpp::XmlElement *stanza);
  private:
    Loop* loop_;
    DISALLOW_EVIL_CONSTRUCTORS(XmppServiceTask);
};

}  // namespace tyrion

#endif  // _TYRION_XMPP_SERVICE_TASK_H_
