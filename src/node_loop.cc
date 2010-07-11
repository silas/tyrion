/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include "node_loop.h"
#include "node_service_handler.h"
#include "node_utils.h"

namespace tyrion {

NodeLoop::NodeLoop(NodeServiceHandler* service_handler) : BaseLoop() {
  track_ = 0;
}

void Loop::Restart() {
  Post(this, MSG_RESTART);
}

void Loop::Reload() {
  Post(this, MSG_RELOAD);
}

void NodeLoop::DoReload() {
  if (NodeReload()) {
    TLOG(INFO) << "Reloading...";
    Restart();
  } else {
    TLOG(WARNING) << "Unable to reload.";
  }
}

void Loop::DoRestart(bool delay = true) {
  if (state_ == RESTARTING)
    return;
  state_ = RESTARTING;
  // TODO(silas): figure out proper method to unwind TaskRunner so
  // InternalRunTasks never segfaults
  if (pump_ != NULL) {
    delete pump_;
    pump_ = NULL;
  }
  if (delay) {
    TLOG(INFO) << "Reconnecting in " << retry_ << " seconds...";
    PostDelayed(retry_ * 1000, this, MSG_LOGIN);
    if (retry_ < MAX_RECONNECT_TIMEOUT) retry_ *= 2;
  } else {
    TLOG(INFO) << "Reconnecting...";
    Post(this, MSG_LOGIN);
  }
}

void NodeLoop::DoRequest(ServiceData* service) {
  if (track_ <= 10) {
    track_++;
    service->data()->set_retry(0);
    service_handler_->Request(service->data());
    delete service;
  } else {
    int retry = service->data()->Retry();
    PostDelayed(retry * 2000, this, MSG_REQUEST, service);
  }
}

void NodeLoop::DoResponse(ServiceData* service) {
  if (state_ == RUNNING && pump_ != NULL &&
      pump_->client() != NULL &&
      pump_->client()->GetState() == txmpp::XmppEngine::STATE_OPEN) {
    track_--;
    const txmpp::XmlElement* iq = service->data()->Response();
    pump_->SendStanza(iq);
    delete iq;
    delete service->data();
    delete service;
  } else {
    int retry = service->data()->Retry();
    TLOG(WARNING) << "Retrying service response in " << retry << " seconds ("
                  << service->data()->id() << ")";
    PostDelayed(retry * 1000, this, MSG_RESPONSE, service);
  }
}

}  // namespace tyrion
