/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_NODE_XMPP_SERVICE_TASK_H_
#define _TYRION_NODE_XMPP_SERVICE_TASK_H_

#include <txmpp/taskparent.h>
#include <txmpp/xmpptask.h>

namespace tyrion {

class NodeLoop;

class NodeXmppServiceTask : public txmpp::XmppTask {
  public:
    explicit NodeXmppServiceTask(NodeLoop* loop, txmpp::TaskParent *parent);
    virtual ~NodeXmppServiceTask() {}
    virtual int ProcessStart();
    virtual int ProcessResponse();
    bool HandleStanza(const txmpp::XmlElement *stanza);
  private:
    NodeLoop* loop_;
    DISALLOW_EVIL_CONSTRUCTORS(NodeXmppServiceTask);
};

}  // namespace tyrion

#endif  // _TYRION_NODE_XMPP_SERVICE_TASK_H_
