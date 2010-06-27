/*
 * Tyrion
 * Copyright 2010, Silas Sewell
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
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

#include "client_loop.h"

namespace tyrion {

ClientLoop::ClientLoop() : BaseLoop() {
  track = 0;
}

ClientLoop* ClientLoop::instance_ = NULL;

ClientLoop* ClientLoop::Instance() {
  if (!instance_)
    instance_ = new ClientLoop;
  return instance_;
}

void ClientLoop::DoRestart(int delay) {
  DoShutdown();
}

void ClientLoop::DoRequest(ServiceData* service) {
  if (state_ == RUNNING && pump_ != NULL &&
      pump_->client() != NULL &&
      pump_->client()->GetState() == txmpp::XmppEngine::STATE_OPEN) {
    track++;
    delete service->data();
    delete service;
  } else {
    int retry = service->data()->Retry();
    if (retry > 6)
      TLOG(WARNING) << "Retrying service request in " << retry << " seconds ("
                    << service->data()->id() << ")";
    PostDelayed(retry * 1000, this, MSG_RESPONSE, service);
  }
}

void ClientLoop::DoResponse(ServiceData* service) {
  track--;
}

}  // namespace tyrion