/*
 * Tyrion
 * Copyright 2010 The Tyrion Authors. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The names of the authors may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
