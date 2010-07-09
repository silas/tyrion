/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _PROCESS_MANAGER_H_
#define _PROCESS_MANAGER_H_

#include <sys/select.h>
#include <txmpp/messagequeue.h>
#include <txmpp/taskrunner.h>
#include <txmpp/thread.h>
#include <txmpp/time.h>

#include "loop.h"

namespace tyrion {

class ProcessManager : public txmpp::MessageHandler, public txmpp::TaskRunner {
  public:
    enum Message {
      MSG_NEW = 1,
      MSG_POLL,
      MSG_DONE,
    };
    typedef MessageDataType<Int> ProcessData;
    typedef std::vector<Process*> ProcessList;

    ProcessManager();

    void WakeTasks();
    void OnMessage(txmpp::Message *pmsg);
    int64 CurrentTime();

  private:
    void DoNew(ServiceData* data);
    void DoPoll();
    void DoDone();
    fd_set rfds_;
    int highest_fd_;
    ProcessList list_;
};

}  // namespace tyrion

#endif  // _PROCESS_MANAGER_H_
