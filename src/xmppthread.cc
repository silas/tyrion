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

#include "xmppthread.h"

#include <txmpp/prexmppauthimpl.h>
#include <txmpp/xmppasyncsocketimpl.h>
#include <txmpp/xmppclientsettings.h>

namespace tyrion {
namespace {

const uint32 MSG_LOGIN = 1;
const uint32 MSG_DISCONNECT = 2;

struct LoginData : public txmpp::MessageData {
  LoginData(const txmpp::XmppClientSettings& s) : xcs(s) {}
  virtual ~LoginData() {}
  txmpp::XmppClientSettings xcs;
};

} // namespace

XmppThread::XmppThread() {
  pump_ = new XmppPump(this);
}

XmppThread::~XmppThread() {
  delete pump_;
}

void XmppThread::ProcessMessages(int cms) {
  txmpp::Thread::ProcessMessages(cms);
}

void XmppThread::Login(const txmpp::XmppClientSettings& xcs) {
  Post(this, MSG_LOGIN, new LoginData(xcs));
}

void XmppThread::Disconnect() {
  Post(this, MSG_DISCONNECT);
}

void XmppThread::OnStateChange(txmpp::XmppEngine::State state) {
}

void XmppThread::OnMessage(txmpp::Message* pmsg) {
  switch (pmsg->message_id) {
    case MSG_LOGIN: {
      assert(pmsg->pdata);
      LoginData* data = reinterpret_cast<LoginData*>(pmsg->pdata);
      pump_->DoLogin(data->xcs, new txmpp::XmppAsyncSocketImpl(true),
                     new txmpp::PreXmppAuthImpl());
      delete data;
      }
      break;
    case MSG_DISCONNECT:
      pump_->DoDisconnect();
      break;
    default:
      assert(false);
  }
}

}  // namespace tyrion
