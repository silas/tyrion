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
#include "node_validators.h"
#include "setting.h"
#include "tyrion.h"

namespace tyrion {
namespace node {

Node::Node() {
  debug_ = false;

  pthread_mutex_init(&mutex_, NULL);
  pthread_create(&handler_, NULL, tyrion::node::signal::SignalHandler, NULL);

  sigemptyset(&set_);
  sigaddset(&set_, SIGHUP);

  sigprocmask(SIG_BLOCK, &set_, NULL);
}

Node::~Node() {
  pthread_mutex_destroy(&mutex_);

  delete(xmpp_);
  pthread_join(handler_, NULL);

  if (instance_)
    delete(instance_);
}

void Node::Reload() {
  if (!Valid()) {
    LOG(ERROR) << "Not reloading.";
    return;
  }

  xmpp_->Restart();

  pthread_mutex_lock(&mutex_);

  Setting::Instance()->Reload();
  Acl::Instance()->Reload();

  xmpp_->Init();

  pthread_mutex_unlock(&mutex_);
}

bool Node::Valid() {

  NodeSettingValidator setting = NodeSettingValidator(config_path_);
  setting.Validate();
  if (setting.HasError()) {
    LOG(ERROR) << "Unable to load settings file.";
  } else if (setting.IsFatal()) {
    setting.ReportIssues();
    return false;
  }

  NodeAclValidator acl = NodeAclValidator(setting.Get("general", "acl"));
  acl.Validate();
  if (acl.HasError()) {
    LOG(ERROR) << "Unable to load ACL file.";
  } else if (acl.IsFatal()) {
    acl.ReportIssues();
    return false;
  }

  return true;
}

bool Node::Setup() {

  Setting::Instance()->File(config_path_);
  if (Setting::Instance()->HasError()) {
    std::cerr << "Unable to load settings file." << std::endl;
    return false;
  }

  NodeSettingValidator setting = NodeSettingValidator(config_path_);
  setting.Validate();
  if (setting.IsFatal()) {
    setting.ReportIssues();
    return false;
  }

  Acl::Instance()->File(Setting::Instance()->Get("general", "acl"));
  if (Acl::Instance()->HasError()) {
    std::cerr << "Unable to load ACL file." << std::endl;
    return false;
  }

  std::string log_path = Setting::Instance()->Get("general", "log");
  Logging *logging = Logging::Instance();
  if (debug_) {
    logging->SetLevel(DEBUG);
  } else if (!log_path.empty() && !logging->SetFile(log_path)) {
    std::cerr << "Unable to open log file." << std::endl;
    return false;
  } else {
    logging->SetStderr(false);
  }

  return true;
}

void Node::Run() {
  if (!xmpp_) xmpp_ = new Xmpp();

  bool reconnect = true;
  while (reconnect) {

    while (xmpp_->state() == Xmpp::Reload) usleep(100000);

    pthread_mutex_lock(&mutex_);

    reconnect = xmpp_->state() != Xmpp::Shutdown;
    if (reconnect && xmpp_->state() != Xmpp::None) {
      LOG(INFO) << "Reconnecting in " << NODE_RECONNECT << " seconds...";
      sleep(NODE_RECONNECT);
    }

    xmpp_->Connect();
    pthread_mutex_unlock(&mutex_);
  }
}

Node* Node::instance_ = NULL;

Node* Node::Instance() {
  if (!instance_)
    instance_ = new Node;
  return instance_;
}

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
        Node::Instance()->Reload();
        break;
    }
  }
}

}  // namespace tyrion::node::signal

} }  // namespace tyrion::node
