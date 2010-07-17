/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _NODE_SERVICE_HANDLER_H_
#define _NODE_SERVICE_HANDLER_H_

#include <sys/select.h>
#include <list>
#include <txmpp/messagequeue.h>
#include <txmpp/taskrunner.h>
#include <txmpp/thread.h>
#include <txmpp/time.h>
#include "basic.h"
#include "constants.h"
#include "node_envelope.h"
#include "node_process.h"

namespace tyrion {

class NodeServiceHandlerData : public txmpp::MessageData {
 public:
  NodeServiceHandlerData(NodeEnvelope* envelope, NodeProcess* process = NULL)
      : envelope_(envelope), process_(process) {}
  NodeEnvelope* envelope() { return envelope_; }
  NodeProcess* process() { return process_; }
  void set_process(NodeProcess* process) { process_ = process; }
 private:
  NodeEnvelope* envelope_;
  NodeProcess* process_;
  DISALLOW_EVIL_CONSTRUCTORS(NodeServiceHandlerData);
};

class NodeServiceHandler : public txmpp::MessageHandler,
                           public txmpp::TaskRunner,
                           public txmpp::Thread {
  public:
    enum Message {
      MSG_REQUEST = 1,
      MSG_POLL,
      MSG_RESPONSE,
      MSG_HANDLE_RESPONSE
    };
    typedef NodeServiceHandlerData ServiceData;
    typedef MessageDataType<NodeEnvelope> EnvelopeData;
    typedef std::list<ServiceData*> ServiceList;

    NodeServiceHandler();

    void Request(NodeEnvelope* envelope);

    void WakeTasks();
    void OnMessage(txmpp::Message *pmsg);
    int64 CurrentTime();

    inline NodeLoop* loop() { return loop_; }
    inline void set_loop(NodeLoop* loop) { loop_ = loop; }

  private:
    void DoRequest(ServiceData* data);
    void DoHandleResponse(ServiceData* data);
    void DoResponse(EnvelopeData* data);
    void DoPoll();
    bool Poll(int timeout = 0);
    fd_set rfds_;
    int highest_fd_;
    ServiceList list_;
    int polling_;
    NodeLoop* loop_;
    DISALLOW_EVIL_CONSTRUCTORS(NodeServiceHandler);
};

}  // namespace tyrion

#endif  // _NODE_SERVICE_HANDLER_H_
