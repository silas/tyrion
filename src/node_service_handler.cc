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
  Post(this, MSG_REQUEST, new ServiceData(envelope));
}

void NodeServiceHandler::WakeTasks() {
  txmpp::Thread::Current()->Post(this);
}

void NodeServiceHandler::DoRequest(ServiceData* service) {
  bool issue = false;
  NodeEnvelope* envelope = service->data();

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

  envelope->set_process(new NodeProcess(envelope->Path(), false, timeout));

  if (!issue && !user.empty() && !envelope->process()->set_user(user)) {
    envelope->append_error(
        CreateError("user.lookup", "Unable to find user '" + user + "'"));
    issue = true;
  }

  if (!issue && !group.empty() && !envelope->process()->set_group(group)) {
    envelope->append_error(
        CreateError("group.lookup", "Unable to find group '" + group + "'"));
    issue = true;
  }

  if (!issue) {
    envelope->process()->Run();
    envelope->process()->Write(envelope->input());

    FD_SET(envelope->process()->outfd[NodeProcess::Stdout][0], &rfds_);
    FD_SET(envelope->process()->outfd[NodeProcess::Stderr][0], &rfds_);

    list_.push_back(envelope);

    Post(this, MSG_POLL);

    delete service;
  } else {
    Post(this, MSG_RESPONSE, service);
  }
}

void NodeServiceHandler::DoResponse(ServiceData* service) {
  NodeEnvelope* envelope = service->data();

  size_t remove = 0;
  int highest_fd = 0;

  NodeEnvelope* e;
  for(size_t x = 0; x < list_.size(); x++) {
    e = list_[x];
    if (e == envelope) {
      remove = x;
    } else {
      // Get highest fd that are not the two we're removing
      if (e->process()->outfd[NodeProcess::Stdout][0] >= highest_fd)
        highest_fd = e->process()->outfd[NodeProcess::Stdout][0];
      if (e->process()->outfd[NodeProcess::Stderr][0] >= highest_fd)
        highest_fd = e->process()->outfd[NodeProcess::Stderr][0];
    }
  }

  highest_fd_ = highest_fd;
  if (remove > 0) {
    FD_CLR(envelope->process()->outfd[NodeProcess::Stdout][0], &rfds_);
    FD_CLR(envelope->process()->outfd[NodeProcess::Stderr][0], &rfds_);

    envelope->set_code(envelope->process()->Close());

    list_.erase(list_.begin() + remove);
  }

  delete envelope->process();

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
    for (int i = NodeProcess::Stdout; i <= NodeProcess::Stderr; i++) {
      // check if ready for reading
      if (FD_ISSET(e->process()->outfd[i][0], &rfds)) {
        char input[PROCESS_BUFFER];
        int rc = read(e->process()->outfd[i][0], input, PROCESS_BUFFER-1);

        if (rc == 0) {
          // got eof for this fd
          e->process()->outfdeof[i] = true;
        } else {
          // update output/error in process
          input[rc] = 0;
          if (NodeProcess::Stdout == i) {
            e->set_output(e->output() + input);
          } else {
            e->set_error(e->error() + input);
          }
        }
      }
    }
    if (e->process()->Done()) {
      // send done message
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
