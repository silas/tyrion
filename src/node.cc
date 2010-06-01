/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "node.h"

#include <string>
#include <iostream>
#include "logging.h"
#include "node_acl.h"
#include "setting.h"
#include "tyrion.h"

namespace tyrion {
namespace node {

namespace signal {

void *SignalHandler(void *arg) {
  int sig;
  sigset_t set;

  sigemptyset(&set);
  sigaddset(&set, SIGHUP);

  while (true) {
    sigwait(&set, &sig);

    switch (sig) {
      case SIGHUP:
        Setting::Instance()->Reload();
        Acl::Instance()->Reload();
        break;
    }
  }
}

}  // namespace tyrion::node::signal

Node::Node() {
  config_path_ = tyrion::CONFIG_PATH;
  debug_ = false;

  pthread_create(&handler_, NULL, tyrion::node::signal::SignalHandler, NULL);

  sigemptyset(&set_);
  sigaddset(&set_, SIGHUP);

  sigprocmask(SIG_BLOCK, &set_, NULL);
}

Node::~Node() {
  delete(xmpp_);
  pthread_join(handler_, NULL);

  if (instance_)
    delete(instance_);
}

bool Node::SetupSetting() {
  Setting::Instance()->File(config_path_);

  return !Setting::Instance()->HasError();
}

bool Node::SetupAcl() {
  std::string path = Setting::Instance()->Get("general", "acl", ACL_PATH);

  Acl::Instance()->File(path);

  return !Acl::Instance()->HasError();
}

bool Node::SetupLogging() {
  std::string path = Setting::Instance()->Get("general", "log", LOG_PATH);
  Logging *logging = Logging::Instance();

  if (debug_) {
    logging->Level(DEBUG);
    logging->Stderr(true);
  } else if (!path.empty() && !logging->File(path)) {
    return false;
  }

  return true;
}

void Node::Run() {
  bool reconnect = true;
  if (!xmpp_) xmpp_ = new Xmpp();

  while (reconnect) {
    xmpp_->Connect();
    reconnect = xmpp_->state() != Xmpp::Shutdown;
    if (reconnect) {
      LOG(INFO) << "Reconnecting in " << NODE_RECONNECT << " seconds...";
      sleep(NODE_RECONNECT);
    }
  }
}

Node* Node::instance_ = NULL;

Node* Node::Instance() {
  if (!instance_)
    instance_ = new Node;
  return instance_;
}

} }  // namespace tyrion::node
