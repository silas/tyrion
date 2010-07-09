/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _PROCESS_MANAGER_H_
#define _PROCESS_MANAGER_H_

#include <sys/select.h>
#include <vector>

#include <txmpp/messagequeue.h>
#include <txmpp/taskrunner.h>
#include <txmpp/thread.h>
#include <txmpp/time.h>

#include "constants.h"
#include "loop.h"
#include "node_envelope.h"
#include "node_process.h"

namespace tyrion {

class NodeServiceHandler : public txmpp::MessageHandler, public txmpp::TaskRunner, public txmpp::MessageQueue {
  public:
    enum Message {
      MSG_REQUEST = 1,
      MSG_POLL,
      MSG_RESPONSE
    };
    typedef MessageDataType<NodeEnvelope> ServiceData;
    typedef std::vector<NodeEnvelope*> ServiceList;

    NodeServiceHandler();

    void Request(NodeEnvelope* envelope);

    void WakeTasks();
    void OnMessage(txmpp::Message *pmsg);
    int64 CurrentTime();

  private:
    void DoRequest(ServiceData* data);
    void DoResponse(ServiceData* data);
    void DoPoll();
    fd_set rfds_;
    int highest_fd_;
    ServiceList list_;
};

}  // namespace tyrion

#endif  // _PROCESS_MANAGER_H_
