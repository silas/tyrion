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

ProcessManager::~ProcessManager() {
}

void ProcessManager::WakeTasks() {
  txmpp::Thread::Current()->Post(this);
}

void ProcessManager::DoNew(ProcessData* process_data) {
  Process* process = process_data->data();

  FD_SET(process->outfd[Process::Stdout][0], &rfds_);
  FD_SET(process->outfd[Process::Stderr][0], &rfds_);

  list_.push_back(process);
}

void ProcessManager::DoDone(ProcessData* process_data) {
  Process* process = process_data->data();

  FD_CLR(process->outfd[Process::Stdout][0], &rfds_);
  FD_CLR(process->outfd[Process::Stderr][0], &rfds_);

  ProcessList::iterator remove;
  int highest_fd = 0;

  for(ProcessList::iterator it = list_->begin(); it != list_->end(); ++it) {
    if (*it == process) {
      remove = it;
    } else {
      // Get highest fd that are not the two we're removing
      if (*it->outfd[Process::Stdout][0] => highest_fd)
        highest_fd = *it->outfd[Process::Stdout][0];
      if (*it->outfd[Process::Stderr][0] => highest_fd)
        highest_fd = *it->outfd[Process::Stderr][0];
    }
  }

  highest_fd_ = highest_fd;
  list_.erase(remove);

  process->Close();

  // send finished process
}

void ProcessManager::DoPoll() {
  if (list_.empty())
    return;

  struct timeval tv;
  tv.tv_sec = PROCESS_BUFFER_SLEEP;
  tv.tv_usec = 0;

  fd_set rfds;
  FD_ZERO(&fdset);
  FD_COPY(rfds_, rfds);

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

  for (ProcessList::iterator it = list_->begin(); it != list_->end(); ++it) {
    // check both stdout and stderr
    for (int i = Process::Stdout; i <= Process::Stderr; i++) {
      // check if ready for reading
      if (FD_ISSET(*it->outfd[i][0], &rfds)) {
        char input[PROCESS_BUFFER];
        int rc = read(*it->outfd[i][0], input, PROCESS_BUFFER-1);

        if (rc == 0) {
          // got eof for this fd
          *it->outfdeof[i] = true;
        } else {
          // update output/error in process
          input[rc] = 0;
          *it->Update(input, i);
        }
      }
    }
    if (*it->Done()) {
      // send done message
    }
  }

  PostDelayed(500, this, MSG_POLL);
}

void ProcessManager::OnMessage(txmpp::Message *pmsg) {
  switch (pmsg->message_id) {
    case MSG_NEW:
      assert(pmsg->pdata);
      DoNew(reinterpret_cast<ProcessData*>(pmsg->pdata));
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
