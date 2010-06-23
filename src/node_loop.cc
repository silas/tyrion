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

#include "node_loop.h"

#include <csignal>
#include <errno.h>
#include <sstream>
#include <txmpp/prexmppauthimpl.h>
#include <txmpp/xmppasyncsocketimpl.h>
#include <txmpp/xmppclientsettings.h>
#include <txmpp/xmppengine.h>
#include "constants.h"
#include "logging.h"
#include "node_service_handler.h"
#include "node_settings.h"
#include "utils.h"

namespace tyrion {

NodeLoop* NodeLoop::instance_ = NULL;

NodeLoop* NodeLoop::Instance() {
  if (!instance_)
    instance_ = new NodeLoop;
  return instance_;
}

NodeLoop::NodeLoop() {
  pump_ = NULL;
  state_ = NONE;
}

NodeLoop::~NodeLoop() {
  if (pump_ != NULL) delete pump_;
}

void NodeLoop::ProcessMessages(int cms) {
  txmpp::Thread::ProcessMessages(cms);
}

void NodeLoop::Login() {
  Post(this, MSG_LOGIN);
}

void NodeLoop::Restart() {
  Post(this, MSG_RESTART);
}

void NodeLoop::Disconnect() {
  Post(this, MSG_DISCONNECT);
}

void NodeLoop::Request(NodeServiceEnvelope* envelope) {
  Post(this, MSG_REQUEST, new ServiceData(envelope));
}

void NodeLoop::Response(NodeServiceEnvelope* envelope) {
  Post(this, MSG_RESPONSE, new ServiceData(envelope));
}

void NodeLoop::DoLogin() {
  txmpp::Jid jid(tyrion::NodeSettings::Instance()->Get("xmpp", "jid"));

  txmpp::InsecureCryptStringImpl password;
  password.password() = tyrion::NodeSettings::Instance()->Get("xmpp",
                                                              "password");

  txmpp::XmppClientSettings settings;
  settings.set_user(jid.node());
  settings.set_pass(txmpp::CryptString(password));
  settings.set_host(jid.domain());
  settings.set_resource(jid.resource());
  settings.set_use_tls(true);

  settings.set_server(txmpp::SocketAddress(
      tyrion::NodeSettings::Instance()->Has("xmpp", "server") ?
          tyrion::NodeSettings::Instance()->Get("xmpp", "server") :
          jid.domain(),
      tyrion::NodeSettings::Instance()->GetInt("xmpp", "port", 5222)
  ));

  if (pump_ == NULL) pump_ = new NodeXmppPump(this);

  state_ = RUNNING;

  txmpp::XmppAsyncSocketImpl* socket =
      new txmpp::XmppAsyncSocketImpl(true);
  socket->SignalCloseEvent.connect(this, &NodeLoop::OnSocketClose);

  TLOG(INFO) << "Connecting...";

  pump_->DoLogin(settings, socket, new txmpp::PreXmppAuthImpl());
}

void NodeLoop::DoDisconnect() {
  TLOG(INFO) << "Disconnecting...";
  state_ = NONE;
  if (pump_ == NULL) return;
  pump_->DoDisconnect();
}

void NodeLoop::DoRestart() {
  TLOG(INFO) << "Reconnecting in " << RECONNECT_TIMEOUT / 1000 << " seconds...";
  state_ = NONE;
  if (pump_ != NULL) {
    delete pump_;
    pump_ = NULL;
  }
  PostDelayed(RECONNECT_TIMEOUT, this, MSG_LOGIN);
}

void NodeLoop::DoShutdown() {
  raise(SIGINT);
}

void NodeLoop::DoRequest(ServiceData* service) {
  NodeServiceHandler *sh = new NodeServiceHandler(service->data());
  CreateThread(NodeLoop::DoRequestInThread, (void *)sh);
  delete service;
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
    const txmpp::XmlElement* iq = service->data()->Response();
    pump_->SendStanza(iq);
    delete iq;
    delete service->data();
    delete service;
  } else {
    TLOG(WARNING) << "Retrying service response in "
                  << SERVICE_RETRY_TIMEOUT / 1000 << " seconds ("
                  << service->data()->id() << ")";
    PostDelayed(SERVICE_RETRY_TIMEOUT, this, MSG_RESPONSE, service);
  }
}

void NodeLoop::OnMessage(txmpp::Message* pmsg) {
  switch (pmsg->message_id) {
    case MSG_LOGIN:
      DoLogin();
      break;
    case MSG_RESTART:
      DoRestart();
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

void NodeLoop::OnSocketClose(int code) {
  state_ = ERROR;
  bool restart = true;

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
      restart = false;
      break;
  }

  Post(this, restart ? MSG_RESTART : MSG_SHUTDOWN);
}

void NodeLoop::OnStateChange(txmpp::XmppEngine::State state, int code) {
  if (state == txmpp::XmppEngine::STATE_CLOSED) {
    state_ = ERROR;
    bool restart = true;

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
        restart = false;
        break;
      case txmpp::XmppEngine::ERROR_UNAUTHORIZED:
        TLOG(ERROR) << "Authorization failed.";
        restart = false;
        break;
      case txmpp::XmppEngine::ERROR_TLS:
        TLOG(ERROR) << "TLS could not be negotiated.";
        break;
      case txmpp::XmppEngine::ERROR_AUTH:
        TLOG(ERROR) << "Authentication could not be negotiated.";
        restart = false;
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
        restart = false;
        break;
    }

    Post(this, restart ? MSG_RESTART : MSG_SHUTDOWN);
  }
}

}  // namespace tyrion
