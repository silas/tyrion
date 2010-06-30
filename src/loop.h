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

#ifndef _TYRION_LOOP_H_
#define _TYRION_LOOP_H_

#include <csignal>
#include <errno.h>
#include <sstream>
#include <iostream>
#include <txmpp/prexmppauthimpl.h>
#include <txmpp/thread.h>
#include <txmpp/xmppasyncsocketimpl.h>
#include <txmpp/xmppclientsettings.h>
#include <txmpp/xmppengine.h>
#include "constants.h"
#include "envelope.h"
#include "logging.h"
#include "utils.h"
#include "xmpp_pump.h"

namespace tyrion {

template <class E, class S, class P>
class Loop : public txmpp::Thread, XmppPumpNotify,
             public txmpp::MessageHandler,
             public txmpp::has_slots<> {
  public:
    enum Message {
      MSG_LOGIN = 1,
      MSG_OPEN,
      MSG_DISCONNECT,
      MSG_RESTART,
      MSG_RELOAD,
      MSG_SHUTDOWN,
      MSG_REQUEST,
      MSG_RESPONSE
    };
    enum State {
      NONE = 0,
      RUNNING,
      RESTARTING,
      STOPPED,
      ERROR
    };
    typedef txmpp::TypedMessageData<E*> ServiceData;

    ~Loop() { if (pump_ != NULL) delete pump_; }

    void Login() {
      Post(this, MSG_LOGIN);
    }

    void Restart() {
      Post(this, MSG_RESTART);
    }

    void Reload() {
      Post(this, MSG_RELOAD);
    }

    void Disconnect() {
      Post(this, MSG_DISCONNECT);
    }

    void Request(E* envelope) {
      Post(this, MSG_REQUEST, new ServiceData(envelope));
    }

    void Response(E* envelope) {
      Post(this, MSG_RESPONSE, new ServiceData(envelope));
    }

    void ProcessMessages(int cms) {
      txmpp::Thread::ProcessMessages(cms);
    }

    inline txmpp::XmppClient* client() { return pump_->client(); }
    inline State state() { return state_; }

    inline pthread_t pthread() { return pthread_; }
    inline void set_pthread(pthread_t pthread) { pthread_ = pthread; };

  protected:
    Loop() {
      pump_ = NULL;
      state_ = NONE;
      retry_ = 1;
    }

    virtual void DoLogin() {
      txmpp::Jid jid(S::Instance()->Get(STR_XMPP, STR_JID));

      txmpp::InsecureCryptStringImpl password;
      password.password() = S::Instance()->Get(STR_XMPP, STR_PASSWORD);

      txmpp::XmppClientSettings settings;
      settings.set_user(jid.node());
      settings.set_pass(txmpp::CryptString(password));
      settings.set_host(jid.domain());
      settings.set_resource(jid.resource());
      settings.set_use_tls(true);

      settings.set_server(txmpp::SocketAddress(
          S::Instance()->Has(STR_XMPP, "server") ?
              S::Instance()->Get(STR_XMPP, "server") :
              jid.domain(),
          S::Instance()->GetInt(STR_XMPP, STR_PORT, XMPP_PORT)
      ));

      if (pump_ == NULL) pump_ = new P(this);

      state_ = RUNNING;

      txmpp::XmppAsyncSocketImpl* socket =
          new txmpp::XmppAsyncSocketImpl(true);
      socket->SignalCloseEvent.connect(this, &Loop::OnSocketClose);

      TLOG(INFO) << "Connecting...";

      pump_->DoLogin(settings, socket, new txmpp::PreXmppAuthImpl());
    }

    virtual void DoOpen() {
      TLOG(INFO) << "Connected.";
      retry_ = 1;
    }

    virtual void DoRestart(bool delay = true) {
      if (state_ == RESTARTING) return;
      state_ = RESTARTING;
      // TODO(silas): figure out proper method to unwind TaskRunner so
      // InternalRunTasks never segfaults
      if (pump_ != NULL) {
        delete pump_;
        pump_ = NULL;
      }
      if (delay) {
        TLOG(INFO) << "Reconnecting in " << retry_ << " seconds...";
        PostDelayed(retry_ * 1000, this, MSG_LOGIN);
        if (retry_ < MAX_RECONNECT_TIMEOUT) retry_ *= 2;
      } else {
        TLOG(INFO) << "Reconnecting...";
        Post(this, MSG_LOGIN);
      }
    }

    virtual void DoReload() {}

    virtual void DoDisconnect() {
      TLOG(INFO) << "Disconnecting...";
      state_ = NONE;
      if (pump_ == NULL) return;
      pump_->DoDisconnect();
    }


    virtual void DoShutdown() {
      if (state_ != ERROR) state_ = STOPPED;
      pthread_kill(pthread_, SIGINT);
    }

    virtual void DoRequest(ServiceData* service) {
    }

    virtual void DoResponse(ServiceData* service) {
    }

    virtual void OnMessage(txmpp::Message* pmsg) {
      switch (pmsg->message_id) {
        case MSG_LOGIN:
          DoLogin();
          break;
        case MSG_OPEN:
          DoOpen();
          break;
        case MSG_RESTART:
          DoRestart();
          break;
        case MSG_RELOAD:
          DoReload();
          break;
        case MSG_DISCONNECT:
          DoDisconnect();
          break;
        case MSG_SHUTDOWN:
          DoShutdown();
          break;
        case MSG_REQUEST:
          assert(pmsg->pdata);
          DoRequest(reinterpret_cast<ServiceData*>(pmsg->pdata));
          break;
        case MSG_RESPONSE:
          assert(pmsg->pdata);
          DoResponse(reinterpret_cast<ServiceData*>(pmsg->pdata));
          break;
        default:
          assert(false);
      }
    }

    void OnSocketClose(int code) {
      state_ = ERROR;

      switch(code) {
        case 0:
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
          break;
      }

      Post(this, MSG_RESTART);
    }

    void OnStateChange(txmpp::XmppEngine::State state, int code = 0) {
      if (state == txmpp::XmppEngine::STATE_OPEN) {
        Post(this, MSG_OPEN);
      } else if (state == txmpp::XmppEngine::STATE_CLOSED) {
        state_ = ERROR;

        switch(code) {
          case txmpp::XmppEngine::ERROR_NONE:
            break;
          case txmpp::XmppEngine::ERROR_XML:
            TLOG(ERROR) << "Malformed XML or encoding error.";
            break;
          case txmpp::XmppEngine::ERROR_STREAM:
            TLOG(ERROR) << "XMPP stream error.";
            break;
          case txmpp::XmppEngine::ERROR_VERSION:
            TLOG(ERROR) << "XMPP version error.";
            break;
          case txmpp::XmppEngine::ERROR_UNAUTHORIZED:
            TLOG(ERROR) << "Authorization failed.";
            break;
          case txmpp::XmppEngine::ERROR_TLS:
            TLOG(ERROR) << "TLS could not be negotiated.";
            break;
          case txmpp::XmppEngine::ERROR_AUTH:
            TLOG(ERROR) << "Authentication could not be negotiated.";
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
            break;
        }

        Post(this, MSG_RESTART);
      }
    }

    P* pump_;
    State state_;
    pthread_t pthread_;
    int retry_;
};


}  // namespace tyrion

#endif  // _TYRION_LOOP_H_
