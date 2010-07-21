/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include "client_loop.h"

#include "client_envelope.h"
#include "client_xmpp_pump.h"
#include "client_xmpp_service_task.h"

namespace tyrion {

ClientLoop::ClientLoop(pthread_t pthread) : Loop(pthread),
    track_(0),
    settings_(NULL),
    pump_(NULL) {
}

ClientLoop::~ClientLoop() {
  if (pump_ != NULL)
    delete pump_;
  if (settings_ != NULL)
    delete settings_;
}

void ClientLoop::Request(ClientEnvelope* envelope) {
  Post(this, MSG_REQUEST, new ServiceData(envelope));
}

void ClientLoop::Response(ClientEnvelope* envelope) {
  Post(this, MSG_RESPONSE, new ServiceData(envelope));
}

void ClientLoop::DoRequest(ServiceData* service) {
  if (state_ == RUNNING && pump_ != NULL && pump_->client() != NULL) {
    track_++;
    ClientXmppServiceTask* task_service = new ClientXmppServiceTask(
        this, pump_->client(), service->data());
    task_service->Start();
    delete service;
  } else {
    int retry = service->data()->Retry();
    if (retry > 6)
      TLOG(WARNING) << "Retrying service request in " << retry << " seconds ("
                    << service->data()->iq_id() << ")";
    PostDelayed(retry * 1000, this, MSG_REQUEST, service);
  }
}

void ClientLoop::DoResponse(ServiceData* service) {
  ClientEnvelope* envelope = service->data();

  if (!envelope->output().empty())
    std::cout << envelope->jid().Str() << " (" << envelope->code() << "): "
              << envelope->output();

  if (!envelope->error().empty())
    std::cerr << envelope->jid().Str() << " (" << envelope->code() << "): "
              << envelope->error();

  if (envelope->output().empty() && envelope->error().empty()) {
    if (envelope->code() == 0) {
      std::cout << envelope->jid().Str() << " (" << envelope->code() << ")"
                << std::endl;
    } else {
      std::cerr << envelope->jid().Str() << " (" << envelope->code() << ")"
                << std::endl;
    }
  }

  delete service;

  if (--track_ <= 0) Disconnect();
}

void ClientLoop::OnMessage(txmpp::Message* message) {
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
      DoShutdown();
      break;
  }
}

void ClientLoop::SetupPump() {
  pump_ = new ClientXmppPump(this);
  Loop::pump_ = pump_;
}

}  // namespace tyrion
