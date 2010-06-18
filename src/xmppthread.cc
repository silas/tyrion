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
#include <errno.h>
#include <sstream>
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

}  // namespace

XmppThread::XmppThread() {
  pump_ = new XmppPump(this);
  state_ = NONE;
}

XmppThread::~XmppThread() {
  delete pump_;
}

void XmppThread::ProcessMessages(int cms) {
  txmpp::Thread::ProcessMessages(cms);
}

void XmppThread::Login(const txmpp::XmppClientSettings& xcs, int delay) {
  if (delay > 0)
    PostDelayed(delay, this, MSG_LOGIN, new LoginData(xcs));
  else
    Post(this, MSG_LOGIN, new LoginData(xcs));
}

void XmppThread::Disconnect() {
  Post(this, MSG_DISCONNECT);
}

void XmppThread::Raise(State state) {
  state_ = state;
  raise(SIGUSR2);
}

void XmppThread::OnStateChange(txmpp::XmppEngine::State state, int code) {
  if (state == txmpp::XmppEngine::STATE_CLOSED) {
    State raise_state = STOPPED_ERROR;

    switch(code) {
      case txmpp::XmppEngine::ERROR_NONE:
        raise_state = STOPPED;
        break;
      case txmpp::XmppEngine::ERROR_XML:
        TLOG(ERROR) << "Malformed XML or encoding error.";
        break;
      case txmpp::XmppEngine::ERROR_STREAM:
        TLOG(ERROR) << "XMPP stream error.";
        break;
      case txmpp::XmppEngine::ERROR_VERSION:
        TLOG(ERROR) << "XMPP version error.";
        raise_state = SHUTDOWN_ERROR;
        break;
      case txmpp::XmppEngine::ERROR_UNAUTHORIZED:
        TLOG(ERROR) << "Authorization failed.";
        raise_state = SHUTDOWN_ERROR;
        break;
      case txmpp::XmppEngine::ERROR_TLS:
        TLOG(ERROR) << "TLS could not be negotiated.";
        break;
      case txmpp::XmppEngine::ERROR_AUTH:
        TLOG(ERROR) << "Authentication could not be negotiated.";
        raise_state = SHUTDOWN_ERROR;
        break;
      case txmpp::XmppEngine::ERROR_BIND:
        TLOG(ERROR) << "Resource or session binding could not be negotiated.";
        break;
      case txmpp::XmppEngine::ERROR_CONNECTION_CLOSED:
        TLOG(ERROR) << "Connection closed by output handler.";
        break;
      case txmpp::XmppEngine::ERROR_DOCUMENT_CLOSED:
        TLOG(ERROR) << "Closed by </stream:stream>.";
        break;
      case txmpp::XmppEngine::ERROR_SOCKET:
        TLOG(ERROR) << "Socket error.";
        break;
      case txmpp::XmppEngine::ERROR_NETWORK_TIMEOUT:
        TLOG(ERROR) << "Network timed out.";
        break;
      default:
        TLOG(ERROR) << "Unknown shutdown error: " << code;
        raise_state = SHUTDOWN_ERROR;
        break;
    }

    Raise(raise_state);
  }
}

void XmppThread::SocketClose(int code) {
  State raise_state = STOPPED_ERROR;

  switch(code) {
    case 0:
      raise_state = STOPPED;
      break;
    case ENETDOWN:
      TLOG(ERROR) << "Network is down.";
      break;
    case ENETUNREACH:
      TLOG(ERROR) << "Network is unreachable.";
      break;
    case ENETRESET:
      TLOG(ERROR) << "Network dropped connection on reset.";
      break;
    case ECONNABORTED:
      TLOG(ERROR) << "Software caused connection abort.";
      break;
    case ECONNRESET:
      TLOG(ERROR) << "Connection reset by peer.";
      break;
    case ECONNREFUSED:
      TLOG(ERROR) << "Connection refused.";
      break;
    default:
      TLOG(ERROR) << "Unhandled socket error: " << code;
      raise_state = SHUTDOWN_ERROR;
      break;
  }

  Raise(raise_state);
}

void XmppThread::OnMessage(txmpp::Message* pmsg) {
  switch (pmsg->message_id) {
    case MSG_LOGIN: {
      assert(pmsg->pdata);
      LoginData* data = reinterpret_cast<LoginData*>(pmsg->pdata);
      state_ = STARTED;
      txmpp::XmppAsyncSocketImpl* socket =
          new txmpp::XmppAsyncSocketImpl(true);
      socket->SignalCloseEvent.connect(this, &XmppThread::SocketClose);
      pump_->DoLogin(data->xcs, socket, new txmpp::PreXmppAuthImpl());
      delete data;
      }
      break;
    case MSG_DISCONNECT:
      pump_->DoDisconnect();
      Raise(SHUTDOWN);
      break;
    default:
      assert(false);
  }
}

}  // namespace tyrion
