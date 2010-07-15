/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include "loop.h"

namespace tyrion {

Loop::Loop(pthread_t pthread) {
  pthread_ = pthread;
  pump_ = NULL;
  retry_ = 1;
  settings_ = NULL;
  state_ = NONE;
}

Loop::~Loop() {
}

void Loop::Login() {
  Post(this, MSG_LOGIN);
}

void Loop::Disconnect() {
  Post(this, MSG_DISCONNECT);
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

  SetupPump();

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
    case MSG_LOGIN:
      DoLogin();
      break;
    case MSG_OPEN:
      DoOpen();
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

void Loop::ProcessMessages(int cms) {
  txmpp::Thread::ProcessMessages(cms);
}

} // namespace tyrion
