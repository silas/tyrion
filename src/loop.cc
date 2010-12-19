/*
 * Copyright (c) 2010, Silas Sewell
 * All rights reserved.
 *
 * This file is subject to the MIT License (see the LICENSE file).
 */

#include "loop.h"

#include <csignal>
#include <errno.h>
#include <txmpp/prexmppauthimpl.h>
#include <txmpp/xmppasyncsocketimpl.h>
#include "settings.h"
#include "utils.h"
#include "xmpp_pump.h"

namespace tyrion {

Loop::Loop(pthread_t pthread) : 
    acls_(NULL),
    service_handler_(NULL),
    settings_(NULL),
    state_(NONE),
    pump_(NULL),
    reconnect_(true),
    retry_(1),
    track_(0),
    pthread_(pthread) {
}

Loop::~Loop() {
  if (pump_ != NULL)
    delete pump_;
  if (settings_ != NULL)
    delete settings_;
  if (acls_ != NULL)
    delete acls_;
}

void Loop::SetReconnect(bool reconnect) {
  Post(this, MSG_SET_RECONNECT, new ReconnectData(reconnect));
}

void Loop::Request(Envelope* envelope) {
  Post(this, MSG_REQUEST, new ServiceData(envelope));
}

void Loop::Response(Envelope* envelope) {
  Post(this, MSG_RESPONSE, new ServiceData(envelope));
}

void Loop::Restart() {
  Post(this, MSG_RESTART);
}

void Loop::Login() {
  Post(this, MSG_LOGIN);
}

void Loop::Disconnect() {
  Post(this, MSG_DISCONNECT);
}

void Loop::DoRequest(ServiceData* service) {
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

void Loop::DoResponse(ServiceData* service) {
  track_--;
  const txmpp::XmlElement* iq = service->data()->Response();
  pump_->SendStanza(iq);
  delete iq;
  delete service->data();
  delete service;
}

void Loop::DoSetReconnect(ReconnectData* reconnect) {
  reconnect_ = reconnect->data();
  delete reconnect;
}

void Loop::DoLogin() {
  txmpp::Jid jid(settings_->Get(SETTING_XMPP, SETTING_JID));

  txmpp::InsecureCryptStringImpl password;
  password.password() = settings_->Get(SETTING_XMPP, SETTING_PASSWORD);

  txmpp::XmppClientSettings xmpp_settings;
  xmpp_settings.set_user(jid.node());
  xmpp_settings.set_pass(txmpp::CryptString(password));
  xmpp_settings.set_host(jid.domain());
  xmpp_settings.set_resource(jid.resource());
  xmpp_settings.set_use_tls(true);

  xmpp_settings.set_server(txmpp::SocketAddress(
      settings_->Has(SETTING_XMPP, SETTING_SERVER) ?
          settings_->Get(SETTING_XMPP, SETTING_SERVER) :
          jid.domain(),
      settings_->GetInt(SETTING_XMPP, SETTING_PORT, XMPP_PORT)
  ));

  pump_ = new XmppPump(this);

  state_ = RUNNING;

  txmpp::XmppAsyncSocketImpl* socket =
      new txmpp::XmppAsyncSocketImpl(true);
  socket->SignalCloseEvent.connect(this, &Loop::OnSocketClose);

  TLOG(INFO) << "Connecting...";

  pump_->DoLogin(xmpp_settings, socket, new txmpp::PreXmppAuthImpl());
}

void Loop::DoOpen() {
  TLOG(INFO) << "Connected.";
  retry_ = 1;
}

void Loop::DoRestart() {
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


void Loop::DoDisconnect() {
  TLOG(INFO) << "Disconnecting...";
  state_ = STOPPED;
  if (pump_ == NULL) return;
  pump_->DoDisconnect();
}

void Loop::DoShutdown() {
  if (state_ != ERROR) state_ = STOPPED;
  pthread_kill(pthread_, SIGINT);
}

void Loop::OnMessage(txmpp::Message* message) {
  switch (message->message_id) {
    case MSG_REQUEST:
      assert(message->pdata);
      DoRequest(reinterpret_cast<ServiceData*>(message->pdata));
      break;
    case MSG_RESPONSE:
      assert(message->pdata);
      DoResponse(reinterpret_cast<ServiceData*>(message->pdata));
      break;
    case MSG_SET_RECONNECT:
      assert(message->pdata);
      DoSetReconnect(reinterpret_cast<ReconnectData*>(message->pdata));
      break;
    case MSG_LOGIN:
      DoLogin();
      break;
    case MSG_OPEN:
      DoOpen();
      break;
    case MSG_RESTART:
      DoRestart();
      break;
    case MSG_CLOSED:
      if (reconnect_) {
        DoRestart();
      }
      break;
    case MSG_DISCONNECT:
      DoDisconnect();
      break;
    case MSG_SHUTDOWN:
      DoShutdown();
      break;
  }
}

void Loop::OnSocketClose(int code) {
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

  Post(this, MSG_CLOSED);
}

void Loop::OnStateChange(txmpp::XmppEngine::State state, int code) {
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

    Post(this, MSG_CLOSED);
  }
}

bool Loop::Ready() {
  return (state_ == RUNNING &&
      pump_ != NULL &&
      pump_->client() != NULL &&
      pump_->client()->GetState() == txmpp::XmppEngine::STATE_OPEN);
}

void Loop::ProcessMessages(int cms) {
  txmpp::Thread::ProcessMessages(cms);
}

} // namespace tyrion
