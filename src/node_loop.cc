/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include "node_loop.h"
#include "node_utils.h"

namespace tyrion {

NodeLoop::NodeLoop() : BaseLoop() {
  track_ = 0;
}

NodeLoop* NodeLoop::instance_ = NULL;

NodeLoop* NodeLoop::Instance() {
  if (!instance_)
    instance_ = new NodeLoop;
  return instance_;
}

void NodeLoop::DoReload() {
  if (NodeReload()) {
    TLOG(INFO) << "Reloading...";
    Restart();
  } else {
    TLOG(WARNING) << "Unable to reload.";
  }
}

void NodeLoop::DoRequest(ServiceData* service) {
  if (track_ <= 10) {
    track_++;
    service->data()->set_retry(0);
    NodeServiceHandler *sh = new NodeServiceHandler(service->data());
    CreateThread(NodeLoop::DoRequestInThread, (void *)sh);
    delete service;
  } else {
    int retry = service->data()->Retry();
    PostDelayed(retry * 2000, this, MSG_REQUEST, service);
  }
}

void *NodeLoop::DoRequestInThread(void *arg) {
  NodeServiceHandler *handler=(NodeServiceHandler*)arg;
  handler->Run();
  delete handler;
  pthread_exit(NULL);
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
