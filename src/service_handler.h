/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_SERVICE_HANDLER_H_
#define _TYRION_SERVICE_HANDLER_H_

#include <list>
#include <txmpp/taskrunner.h>
#include <txmpp/thread.h>
#include "common.h"
#include "envelope.h"

namespace tyrion {

class Process;

class ServiceHandlerData : public txmpp::MessageData {
 public:
  ServiceHandlerData(Envelope* envelope, Process* process = NULL)
      : envelope_(envelope), process_(process) {}
  Envelope* envelope() { return envelope_; }
  Process* process() { return process_; }
  void set_process(Process* process) { process_ = process; }
 private:
  Envelope* envelope_;
  Process* process_;
  DISALLOW_EVIL_CONSTRUCTORS(ServiceHandlerData);
};

class ServiceHandler : public txmpp::MessageHandler,
                           public txmpp::TaskRunner,
                           public txmpp::Thread {
  public:
    enum Message {
      MSG_REQUEST = 1,
      MSG_POLL,
      MSG_RESPONSE,
      MSG_HANDLE_RESPONSE
    };
    typedef ServiceHandlerData ServiceData;
    typedef MessageDataType<Envelope> EnvelopeData;
    typedef std::list<ServiceData*> ServiceList;

    ServiceHandler();

    void Request(Envelope* envelope);

    void WakeTasks();
    void OnMessage(txmpp::Message *pmsg);
    int64 CurrentTime();

    inline Loop* loop() { return loop_; }
    inline void set_loop(Loop* loop) { loop_ = loop; }

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
    Loop* loop_;
    DISALLOW_EVIL_CONSTRUCTORS(ServiceHandler);
};

}  // namespace tyrion

#endif  // _TYRION_SERVICE_HANDLER_H_
