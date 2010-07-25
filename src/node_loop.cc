/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include "node_loop.h"

#include "node_utils.h"
#include "node_xmpp_pump.h"

namespace tyrion {

NodeLoop::NodeLoop(pthread_t pthread) : Loop(pthread),
    track_(0),
    settings_(NULL),
    acls_(NULL),
    pump_(NULL),
    service_handler_(NULL),
    reconnect_(true) {
}

NodeLoop::~NodeLoop() {
  if (pump_ != NULL)
    delete pump_;
  if (settings_ != NULL)
    delete settings_;
  if (acls_ != NULL)
    delete acls_;
}

void NodeLoop::SetReconnect(bool reconnect) {
  Post(this, MSG_SET_RECONNECT, new ReconnectData(reconnect));
}

void NodeLoop::Restart() {
  Post(this, MSG_RESTART);
}

void NodeLoop::Request(NodeEnvelope* envelope) {
  Post(this, MSG_REQUEST, new ServiceData(envelope));
}

void NodeLoop::Response(NodeEnvelope* envelope) {
  Post(this, MSG_RESPONSE, new ServiceData(envelope));
}

void NodeLoop::DoSetReconnect(ReconnectData* reconnect) {
  reconnect_ = reconnect->data();
  delete reconnect;
}

void NodeLoop::DoRestart() {
  if (state_ == RESTARTING)
    return;
  state_ = RESTARTING;
  if (pump_ != NULL) {
    delete pump_;
    pump_ = NULL;
  }
  TLOG(INFO) << "Reconnecting in " << retry_ << " seconds...";
  PostDelayed(retry_ * 1000, this, MSG_LOGIN);
  if (retry_ < MAX_RECONNECT_TIMEOUT)
    retry_ *= 2;
}

void NodeLoop::DoRequest(ServiceData* service) {
  if (track_ <= 1000) {
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
  track_--;
  const txmpp::XmlElement* iq = service->data()->Response();
  pump_->SendStanza(iq);
  delete iq;
  delete service->data();
  delete service;
}

void NodeLoop::OnMessage(txmpp::Message* message) {
  Loop::OnMessage(message);

  switch (message->message_id) {
    case MSG_REQUEST:
      assert(message->pdata);
      DoRequest(reinterpret_cast<ServiceData*>(message->pdata));
      break;
    case MSG_RESPONSE:
      assert(message->pdata);
      DoResponse(reinterpret_cast<ServiceData*>(message->pdata));
      break;
    case MSG_CLOSED:
      if (reconnect_) {
        DoRestart();
      }
      break;
    case MSG_RESTART:
      DoRestart();
      break;
    case MSG_SET_RECONNECT:
      assert(message->pdata);
      DoSetReconnect(reinterpret_cast<ReconnectData*>(message->pdata));
      break;
  }
}

void NodeLoop::SetupPump() {
  pump_ = new NodeXmppPump(this);
  Loop::pump_ = pump_;
}

}  // namespace tyrion
