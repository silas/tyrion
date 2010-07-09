/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include "process_manager.h"

#include <txmpp/logging.h>

namespace tyrion {

ProcessManager::ProcessManager() {
  FD_ZERO(&rfds_);
  highest_fd_ = 0;
}

void ProcessManager::WakeTasks() {
  txmpp::Thread::Current()->Post(this);
}

void ProcessManager::DoNew(ServiceData* service) {
  NodeEnvelope* envelope = service->data();

  FD_SET(envelope->process()->outfd[Process::Stdout][0], &rfds_);
  FD_SET(envelope->process()->outfd[Process::Stderr][0], &rfds_);

  list_.push_back(envelope);
}

void ProcessManager::DoDone(ServiceData* service) {
  NodeEnvelope* envelope = service->data();

  FD_CLR(envelope->process()->outfd[Process::Stdout][0], &rfds_);
  FD_CLR(envelope->process()->outfd[Process::Stderr][0], &rfds_);

  size_t remove = 0;
  int highest_fd = 0;

  NodeEnvelope* e;
  for(size_t x = 0; x < list_.size(); x++) {
    e = list_[x];
    if (e == envelope) {
      remove = x;
    } else {
      // Get highest fd that are not the two we're removing
      if (e->process()->outfd[Process::Stdout][0] >= highest_fd)
        highest_fd = e->process()->outfd[Process::Stdout][0];
      if (e->process()->outfd[Process::Stderr][0] >= highest_fd)
        highest_fd = e->process()->outfd[Process::Stderr][0];
    }
  }

  highest_fd_ = highest_fd;
  if (remove > 0)
    list_.erase(list_.begin() + remove);

  envelope->process()->Close();

  // send finished process
}

void ProcessManager::DoPoll() {
  if (list_.empty())
    return;

  struct timeval tv;
  tv.tv_sec = PROCESS_BUFFER_SLEEP;
  tv.tv_usec = 0;

  fd_set rfds;
  FD_ZERO(&rfds);
  FD_COPY(&rfds_, &rfds);

  int sr = select(highest_fd_ + 1, &rfds, NULL, NULL, &tv);

  // select timeout
  if (sr == 0) {
    PostDelayed(500, this, MSG_POLL);
    return;
  }

  // select error
  if (sr < 0) {
    PostDelayed(500, this, MSG_POLL);
    return;
  }

  NodeEnvelope* e;
  for(size_t x = 0; x < list_.size(); x++) {
    e = list_[x];
    // check both stdout and stderr
    for (int i = Process::Stdout; i <= Process::Stderr; i++) {
      // check if ready for reading
      if (FD_ISSET(e->process()->outfd[i][0], &rfds)) {
        char input[1024];
        int rc = read(e->process()->outfd[i][0], input, 1023);

        if (rc == 0) {
          // got eof for this fd
          e->process()->outfdeof[i] = true;
        } else {
          // update output/error in process
          input[rc] = 0;
          e->process()->Update(input, (Process::ProcessType)i);
        }
      }
    }
    if (e->process()->Done()) {
      // send done message
    }
  }

  PostDelayed(500, this, MSG_POLL);
}

void ProcessManager::OnMessage(txmpp::Message *pmsg) {
  switch (pmsg->message_id) {
    case MSG_NEW:
      assert(pmsg->pdata);
      DoNew(reinterpret_cast<ServiceData*>(pmsg->pdata));
      break;
    case MSG_POLL:
      DoPoll();
      break;
    default:
      assert(false);
  }
}

int64 ProcessManager::CurrentTime() {
  return (int64)txmpp::Time();
}

}  // namespace tyrion
