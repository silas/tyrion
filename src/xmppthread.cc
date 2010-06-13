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

#include <csignal>
#include <txmpp/prexmppauthimpl.h>
#include <txmpp/xmppasyncsocketimpl.h>
#include <txmpp/xmppclientsettings.h>
#include "logging.h"

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
  shutdown_ = false;
  exit_code_ = 0;
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

void XmppThread::OnStateChange(txmpp::XmppEngine::State state, int code) {
  if (state == txmpp::XmppEngine::STATE_CLOSED) {
    switch(code) {
      case txmpp::XmppEngine::ERROR_XML:
        TLOG(WARNING) << "Malformed XML or encoding error.";
        break;
      case txmpp::XmppEngine::ERROR_STREAM:
        TLOG(WARNING) << "XMPP stream error.";
        break;
      case txmpp::XmppEngine::ERROR_VERSION:
        TLOG(WARNING) << "XMPP version error.";
        shutdown_ = true;
        break;
      case txmpp::XmppEngine::ERROR_UNAUTHORIZED:
        TLOG(WARNING) << "Authorization failed.";
        shutdown_ = true;
        break;
      case txmpp::XmppEngine::ERROR_TLS:
        TLOG(WARNING) << "TLS could not be negotiated.";
        break;
      case txmpp::XmppEngine::ERROR_AUTH:
        TLOG(WARNING) << "Authentication could not be negotiated.";
        break;
      case txmpp::XmppEngine::ERROR_BIND:
        TLOG(WARNING) << "Resource or session binding could not be negotiated.";
        break;
      case txmpp::XmppEngine::ERROR_CONNECTION_CLOSED:
        TLOG(WARNING) << "Connection closed by output handler.";
        break;
      case txmpp::XmppEngine::ERROR_DOCUMENT_CLOSED:
        TLOG(WARNING) << "Closed by </stream:stream>.";
        break;
      case txmpp::XmppEngine::ERROR_SOCKET:
        TLOG(WARNING) << "Socket error.";
        break;
      case txmpp::XmppEngine::ERROR_NETWORK_TIMEOUT:
        TLOG(WARNING) << "Network timed out.";
        break;
    }
    if (code > 0) exit_code_ = 1;
    raise(SIGUSR2);
  }
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
      shutdown_ = true;
      break;
    default:
      assert(false);
  }
}

}  // namespace tyrion
