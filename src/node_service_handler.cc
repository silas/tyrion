/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include "node_service_handler.h"

#include <txmpp/logging.h>
#include "node_loop.h"

#define PROCESS_BUFFER 1024

namespace tyrion {

NodeServiceHandler::NodeServiceHandler() {
  FD_ZERO(&rfds_);
  highest_fd_ = 0;
}

void NodeServiceHandler::Request(NodeEnvelope* envelope) {
  Post(this, MSG_REQUEST, new NodeServiceHandlerData(envelope));
}

void NodeServiceHandler::WakeTasks() {
  txmpp::Thread::Current()->Post(this);
}

void NodeServiceHandler::DoRequest(ServiceData* service) {
  NodeEnvelope* envelope = service->envelope();

  bool issue = false;
  std::string config = "service:" + envelope->type();

  std::string user = NodeSettings::Instance()->Get(config, "user");
  bool user_lock = NodeSettings::Instance()->GetBool(config, "user_lock");

  std::string group = NodeSettings::Instance()->Get(config, "group");
  bool group_lock = NodeSettings::Instance()->GetBool(config, "group_lock");

  int timeout = NodeSettings::Instance()->GetInt(config, "timeout");
  bool timeout_lock = NodeSettings::Instance()->GetBool(config, "timeout_lock");

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

  NodeProcess* process = new NodeProcess(envelope->Path(), false, timeout);
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
    FD_SET(process->outfd[NodeProcess::Stdout][0], &rfds_);
    FD_SET(process->outfd[NodeProcess::Stderr][0], &rfds_);

    if (process->outfd[NodeProcess::Stdout][0] > highest_fd_)
      highest_fd_ = process->outfd[NodeProcess::Stdout][0];

    if (process->outfd[NodeProcess::Stderr][0] > highest_fd_)
      highest_fd_ = process->outfd[NodeProcess::Stderr][0];

    process->Run();
    process->Write(envelope->input());

    list_.push_back(service);

    Post(this, MSG_POLL);
  } else {
    Post(this, MSG_RESPONSE, service);
  }
}

void NodeServiceHandler::DoResponse(ServiceData* service) {
  NodeEnvelope* envelope = service->envelope();
  NodeProcess* process = service->process();

  ServiceList::iterator remove = list_.end();
  int highest_fd = 0;

  NodeEnvelope* e = NULL;
  NodeProcess* p = NULL;
  for (ServiceList::iterator it = list_.begin(); it != list_.end(); it++) {
    e = (*it)->envelope();
    p = (*it)->process();
    if (*it == service) {
      remove = it;
    } else {
      // Get highest fd that are not the two we're removing
      if (p->outfd[NodeProcess::Stdout][0] >= highest_fd)
        highest_fd = p->outfd[NodeProcess::Stdout][0];
      if (p->outfd[NodeProcess::Stderr][0] >= highest_fd)
        highest_fd = p->outfd[NodeProcess::Stderr][0];
    }
  }
  highest_fd_ = highest_fd;

  if (remove != list_.end()) {
    envelope->set_code(process->Close());

    FD_CLR(p->outfd[NodeProcess::Stdout][0], &rfds_);
    FD_CLR(p->outfd[NodeProcess::Stderr][0], &rfds_);

    list_.erase(remove);
  }

  delete process;
  delete service;

  NodeLoop::Instance()->Response(envelope);
}

void NodeServiceHandler::DoPoll() {
  if (list_.empty())
    return;

  struct timeval tv;
  tv.tv_sec = PROCESS_BUFFER_SLEEP;
  tv.tv_usec = 0;

  fd_set rfds;
  FD_ZERO(&rfds);
  FD_COPY(&rfds_, &rfds);

  int sr = select(highest_fd_ + 1, &rfds, NULL, NULL, &tv);

  NodeEnvelope* e;
  NodeProcess *p;
  for (ServiceList::iterator it = list_.begin(); it != list_.end(); it++) {
    e = (*it)->envelope();
    p = (*it)->process();
    if (sr > 0) {
      // check both stdout and stderr
      for (int i = NodeProcess::Stdout; i <= NodeProcess::Stderr; i++) {
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
            if (NodeProcess::Stdout == i) {
              e->append_output(data);
            } else {
              e->append_error(data);
            }
          }
        }
      }
    }
    if (p->Done()) {
      Post(this, MSG_RESPONSE, *it);
    }
  }

  PostDelayed(500, this, MSG_POLL);
}

void NodeServiceHandler::OnMessage(txmpp::Message *pmsg) {
  switch (pmsg->message_id) {
    case MSG_REQUEST:
      assert(pmsg->pdata);
      DoRequest(reinterpret_cast<ServiceData*>(pmsg->pdata));
      break;
    case MSG_RESPONSE:
      assert(pmsg->pdata);
      DoResponse(reinterpret_cast<ServiceData*>(pmsg->pdata));
      break;
    case MSG_POLL:
      DoPoll();
      break;
    default:
      assert(false);
  }
}

int64 NodeServiceHandler::CurrentTime() {
  return (int64)txmpp::Time();
}

}  // namespace tyrion
