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
        std::cerr << "Malformed XML or encoding error." << std::endl;
        break;
      case txmpp::XmppEngine::ERROR_STREAM:
        std::cerr << "XMPP stream error." << std::endl;
        break;
      case txmpp::XmppEngine::ERROR_VERSION:
        std::cerr << "XMPP version error." << std::endl;
        shutdown_ = true;
        break;
      case txmpp::XmppEngine::ERROR_UNAUTHORIZED:
        std::cerr << "Authorization failed." << std::endl;
        shutdown_ = true;
        break;
      case txmpp::XmppEngine::ERROR_TLS:
        std::cerr << "TLS could not be negotiated." << std::endl;
        break;
      case txmpp::XmppEngine::ERROR_AUTH:
        std::cerr << "Authentication could not be negotiated." << std::endl;
        break;
      case txmpp::XmppEngine::ERROR_BIND:
        std::cerr << "Resource or session binding could not be negotiated." << std::endl;
        break;
      case txmpp::XmppEngine::ERROR_CONNECTION_CLOSED:
        std::cerr << "Connection closed by output handler." << std::endl;
        break;
      case txmpp::XmppEngine::ERROR_DOCUMENT_CLOSED:
        std::cerr << "Closed by </stream:stream>." << std::endl;
        break;
      case txmpp::XmppEngine::ERROR_SOCKET:
        std::cerr << "Socket error." << std::endl;
        break;
      case txmpp::XmppEngine::ERROR_NETWORK_TIMEOUT:
        std::cerr << "Network timed out." << std::endl;
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
