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
  state_ = Xmpp::None;

  gloox::JID jid(Setting::Instance()->Get("xmpp", "jid"));
  client_ = new gloox::Client(jid, Setting::Instance()->Get("xmpp",
                                                            "password"));

  // Use specified server (otherwise we do a name lookup)
  if (!Setting::Instance()->Get("xmpp", "server").empty())
    client_->setServer(Setting::Instance()->Get("xmpp", "server"));

  // Announce we're a Tyrion client
  client_->registerConnectionListener(this);
  client_->disco()->setVersion("Tyrion", VERSION);
  client_->disco()->setIdentity("client", "bot");

  // Optionally disable SASL
  client_->setSasl(Setting::Instance()->GetBool("xmpp", "sasl", true));

  gloox::StringList ca;
  ca.push_back(Setting::Instance()->Get("xmpp", "cacert"));
  client_->setCACerts(ca);

  // Enable raw XMPP logging
  client_->logInstance().registerLogHandler(gloox::LogLevelDebug,
                                            gloox::LogAreaAll, this);
}

Xmpp::~Xmpp() {
  delete(client_);
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

void Xmpp::Stop() {
  state_ = Xmpp::Shutdown;

  client_->disconnect();
}

void Xmpp::onConnect() {
  state_ = Xmpp::Connected;
  LOG(INFO) << "Connected to XMPP server";
}

void Xmpp::onDisconnect(gloox::ConnectionError e) {
  if (state_ != Xmpp::Shutdown)
    state_ = Xmpp::Disconnected;
  LOG(ERROR) << "Disconnected: " << e;
  if(e == gloox::ConnAuthenticationFailed)
    LOG(ERROR) << "Authenitcation failed: " << client_->authError();
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
