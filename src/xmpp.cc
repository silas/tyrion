/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "xmpp.h"

#include <string>
#include "logging.h"
#include "setting.h"
#include "tyrion.h"

namespace tyrion {

Xmpp::Xmpp() {
  // Create XMPP client
  gloox::JID jid(Setting::Instance()->Get("xmpp", "jid"));
  client_ = new XmppClient(jid, Setting::Instance()->Get("xmpp", "password"));

  // Announce we're a Tyrion client
  client_->registerConnectionListener(this);
  client_->disco()->setVersion("Tyrion", VERSION);
  client_->disco()->setIdentity("client", "bot");

  // Enable raw XMPP logging
  client_->logInstance().registerLogHandler(gloox::LogLevelDebug,
                                            gloox::LogAreaAll, this);

  Init();
}

Xmpp::~Xmpp() {
  if (client_) {
    delete(client_);
    client_ = NULL;
  }
}

void Xmpp::Init() {
  // Setup username and password (reloads)
  gloox::JID jid(Setting::Instance()->Get("xmpp", "jid"));
  client_->setUsername(jid.username());
  client_->setResource(jid.resource());
  client_->setPassword(Setting::Instance()->Get("xmpp", "password"));

  // Use specified server (otherwise we do a name lookup)
  if (!Setting::Instance()->Get("xmpp", "server").empty())
    client_->setServer(Setting::Instance()->Get("xmpp", "server"));

  // Optionally disable SASL
  client_->setSasl(Setting::Instance()->GetBool("xmpp", "sasl", true));

  // Setup CA certificate
  gloox::StringList ca;
  ca.push_back(Setting::Instance()->Get("xmpp", "cacert"));
  client_->setCACerts(ca);

  state_ = Xmpp::None;
}

void Xmpp::Connect() {
  // Setup custom XMPP handlers
  SetupHandlers();

  // XMPP event loop
  if(client_->connect(false)) {
    gloox::ConnectionError ce = gloox::ConnNoError;

    while (ce == gloox::ConnNoError)
      ce = client_->recv();

    LOG(INFO) << "ce: " << ce;
  }

  // Destroy custom managers
  DestroyHandlers();
}

void Xmpp::Stop(bool reload) {
  if (reload) {
    state_ = Xmpp::Reload;
    client_->Restart();
  } else {
    state_ = Xmpp::Shutdown;
    client_->disconnect();
  }
}

void Xmpp::onConnect() {
  state_ = Xmpp::Connected;
  LOG(INFO) << "Connected to XMPP server";
}

void Xmpp::onDisconnect(gloox::ConnectionError e) {
  if (state_ != Xmpp::Shutdown && state_ != Xmpp::Reload)
    state_ = Xmpp::Disconnected;
  LOG(INFO) << "Disconnected: " << e;
  if(e == gloox::ConnAuthenticationFailed) {
    state_ = Xmpp::Error;
    LOG(ERROR) << "Authenitcation failed: " << client_->authError();
  }
}

bool Xmpp::onTLSConnect(const gloox::CertInfo& info) {
  time_t from(info.date_from);
  time_t to(info.date_to);

  LOG(INFO) << "status: " << info.status
            << ", issuer: " << info.issuer
            << ", peer: " << info.server
            << ", protocol: " << info.protocol
            << ", mac: " << info.mac
            << ", cipher: " << info.cipher
            << ", compression: " << info.compression
            << ", from: " << ctime(&from)
            << ", to: " << ctime(&to);

  return true;
}

void Xmpp::handleLog(gloox::LogLevel level, gloox::LogArea area,
                     const std::string& message) {
  LOG(DEBUG) << "level: " << level << ", area: " << area << ", "
             << message;
}

} // namespace tyrion
