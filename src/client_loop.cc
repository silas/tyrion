/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include "client_loop.h"

#include <iostream>
#include "client_xmpp_service_task.h"

namespace tyrion {

ClientLoop::ClientLoop() : Loop() {
  track = 0;
}

void ClientLoop::DoRequest(ServiceData* service) {
  if (state_ == RUNNING && pump_ != NULL && pump_->client() != NULL) {
    track++;
    ClientXmppServiceTask* task_service = new ClientXmppServiceTask(
        this, pump_->client(), service->data());
    task_service->Start();
    delete service;
  } else {
    int retry = service->data()->Retry();
    if (retry > 6)
      TLOG(WARNING) << "Retrying service request in " << retry << " seconds ("
                    << service->data()->id() << ")";
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

  if (--track <= 0) Disconnect();
}

bool ClientLoop::OnMessage(txmpp::Message* message) {
  if (Node::OnMessage(message))
    return true;

  bool handled = true;

  switch (message->message_id) {
    case MSG_REQUEST:
      assert(pmsg->pdata);
      DoRequest(reinterpret_cast<ServiceData*>(message->pdata));
      break;
    case MSG_RESPONSE:
      assert(pmsg->pdata);
      DoResponse(reinterpret_cast<ServiceData*>(message->pdata));
      break;
    case MSG_SOCKET_CLOSED:
      DoShutdown();
      break;
    default:
      handled = false;
  }

  return handled;
}

}  // namespace tyrion
