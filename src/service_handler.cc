/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include "service_handler.h"

#include <sys/select.h>
#include "loop.h"
#include "process.h"
#include "settings.h"
#include "utils.h"

#ifndef FD_COPY
#define FD_COPY(f, t) memcpy(t, f, sizeof(*(f)))
#endif

namespace tyrion {

ServiceHandler::ServiceHandler() :
    rfds_(),
    highest_fd_(0),
    list_(),
    polling_(0),
    loop_(NULL) {
  FD_ZERO(&rfds_);
}

void ServiceHandler::Request(Envelope* envelope) {
  Post(this, MSG_REQUEST, new ServiceHandlerData(envelope));
}

void ServiceHandler::WakeTasks() {
  txmpp::Thread::Current()->Post(this);
}

void ServiceHandler::DoRequest(ServiceData* service) {
  Envelope* envelope = service->envelope();

  bool issue = false;
  std::string config = "service:" + envelope->type();

  std::string user = loop_->settings()->Get(config, "user");
  bool user_lock = loop_->settings()->GetBool(config, "user_lock");

  std::string group = loop_->settings()->Get(config, "group");
  bool group_lock = loop_->settings()->GetBool(config, "group_lock");

  int timeout = loop_->settings()->GetInt(config, "timeout");
  bool timeout_lock = loop_->settings()->GetBool(config, "timeout_lock");

  if (!issue && user_lock) {
    if (user.empty()) {
      envelope->append_error(
          CreateError("user.lock", "Unable to lock user because none set."));
      issue = true;
    }
  } else if (!issue && !envelope->user().empty()) {
    user = envelope->user();
  }

  if (!issue && group_lock) {
    if (group.empty()) {
      envelope->append_error(
          CreateError("group.lock", "Unable to lock group because none set."));
      issue = true;
    }
  } else if (!issue && !envelope->group().empty())
    group = envelope->group();

  if (!issue && timeout_lock) {
    if (!timeout > 0) {
      envelope->append_error(
          CreateError("timeout.lock",
                      "Unable to lock timeout because none set."));
      issue = true;
    }
  } else {
    timeout = envelope->timeout();
  }

  Process* process = new Process(envelope->Path(), false, timeout);
  service->set_process(process);

  if (!issue && !user.empty() && !process->set_user(user)) {
    envelope->append_error(
        CreateError("user.lookup", "Unable to find user '" + user + "'"));
    issue = true;
  }

  if (!issue && !group.empty() && !process->set_group(group)) {
    envelope->append_error(
        CreateError("group.lookup", "Unable to find group '" + group + "'"));
    issue = true;
  }

  if (!issue) {
    FD_SET(process->outfd[Process::Stdout][0], &rfds_);
    FD_SET(process->outfd[Process::Stderr][0], &rfds_);

    if (process->outfd[Process::Stdout][0] > highest_fd_)
      highest_fd_ = process->outfd[Process::Stdout][0];

    if (process->outfd[Process::Stderr][0] > highest_fd_)
      highest_fd_ = process->outfd[Process::Stderr][0];

    process->Run();
    process->Write(envelope->input());

    list_.push_back(service);

    Poll();
  } else {
    Post(this, MSG_HANDLE_RESPONSE, service);
  }
}

void ServiceHandler::DoHandleResponse(ServiceData* service) {
  Envelope* envelope = service->envelope();
  Process* process = service->process();

  ServiceList::iterator remove = list_.end();
  int highest_fd = 0;

  Envelope* e = NULL;
  Process* p = NULL;
  for (ServiceList::iterator it = list_.begin(); it != list_.end(); it++) {
    e = (*it)->envelope();
    p = (*it)->process();
    if (*it == service) {
      remove = it;
    } else {
      // Get highest fd that are not the two we're removing
      if (p->outfd[Process::Stdout][0] >= highest_fd)
        highest_fd = p->outfd[Process::Stdout][0];
      if (p->outfd[Process::Stderr][0] >= highest_fd)
        highest_fd = p->outfd[Process::Stderr][0];
    }
  }
  highest_fd_ = highest_fd;

  if (remove != list_.end()) {
    envelope->set_code(process->Close());

    FD_CLR(process->outfd[Process::Stdout][0], &rfds_);
    FD_CLR(process->outfd[Process::Stderr][0], &rfds_);

    list_.erase(remove);
  }

  delete process;
  delete service;

  Post(this, MSG_RESPONSE, new EnvelopeData(envelope));
}

void ServiceHandler::DoResponse(EnvelopeData* service) {
  // TODO(silas): the Ready call probably isn't thread safe, figure out
  // alternative
  if (loop_->Ready()) {
    loop_->Response(service->data());
    delete service;
  } else {
    int retry = service->data()->Retry();
    TLOG(WARNING) << "Retrying service response in " << retry << " seconds ("
                  << service->data()->iq_id() << ")";
    PostDelayed(retry * 1000, this, MSG_RESPONSE, service);
  }
}

void ServiceHandler::DoPoll() {
  polling_--;
  if (list_.empty())
    return;

  struct timeval tv;
  tv.tv_sec = PROCESS_BUFFER_SLEEP;
  tv.tv_usec = 0;

  fd_set rfds;
  FD_ZERO(&rfds);
  FD_COPY(&rfds_, &rfds);

  int sr = select(highest_fd_ + 1, &rfds, NULL, NULL, &tv);

  Envelope* e;
  Process *p;
  for (ServiceList::iterator it = list_.begin(); it != list_.end(); it++) {
    e = (*it)->envelope();
    p = (*it)->process();
    if (sr > 0) {
      // check both stdout and stderr
      for (int i = Process::Stdout; i <= Process::Stderr; i++) {
        // check if ready for reading
        if (FD_ISSET(p->outfd[i][0], &rfds)) {
          char data[PROCESS_BUFFER];
          int rc = read(p->outfd[i][0], data, PROCESS_BUFFER-1);

          if (rc == 0) {
            // got eof for this fd
            p->outfdeof[i] = true;
          } else {
            // update output/error in process
            data[rc] = 0;
            if (Process::Stdout == i) {
              e->append_output(data);
            } else {
              e->append_error(data);
            }
          }
        }
      }
    }
    if (p->Done()) {
      Post(this, MSG_HANDLE_RESPONSE, *it);
    }
  }

  Poll(PROCESS_POLL_TIMEOUT);
}

bool ServiceHandler::Poll(int timeout) {
  if (polling_ > 0) {
    return false;
  }
  polling_++;
  PostDelayed(timeout, this, MSG_POLL);
  return true;
}

void ServiceHandler::OnMessage(txmpp::Message *pmsg) {
  switch (pmsg->message_id) {
    case MSG_REQUEST:
      assert(pmsg->pdata);
      DoRequest(reinterpret_cast<ServiceData*>(pmsg->pdata));
      break;
    case MSG_HANDLE_RESPONSE:
      assert(pmsg->pdata);
      DoHandleResponse(reinterpret_cast<ServiceData*>(pmsg->pdata));
      break;
    case MSG_RESPONSE:
      assert(pmsg->pdata);
      DoResponse(reinterpret_cast<EnvelopeData*>(pmsg->pdata));
      break;
    case MSG_POLL:
      DoPoll();
      break;
    default:
      assert(false);
  }
}

int64 ServiceHandler::CurrentTime() {
  return (int64)txmpp::Time();
}

}  // namespace tyrion
