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
#include "node_setting_validator.h"
#include "setting.h"
#include "utils.h"
#include "tyrion.h"

namespace tyrion {
namespace node {

Node* Node::instance_ = NULL;

Node::Node() {
  debug_ = false;

  // Create a thread to handle signals
  utils::CreateThread(signal::SignalHandler, NULL);

  // Reload mutex
  pthread_mutex_init(&mutex_, NULL);

  // Ignore SIGHUP here and for all children
  sigfillset(&set_);
  sigaddset(&set_, SIGHUP);
  sigaddset(&set_, SIGINT);
  sigaddset(&set_, SIGTERM);
  pthread_sigmask(SIG_BLOCK, &set_, NULL);
}

Node::~Node() {
  if (xmpp_) {
    pthread_mutex_destroy(&mutex_);
    delete(xmpp_);
    xmpp_ = NULL;
  }
}

void Node::Reload() {
  if (!Valid()) {
    LOG(ERROR) << "Not reloading.";
    return;
  }

  // Restart XMPP client
  xmpp_->Stop(true);

  // Lock mutex so we're not trying to connect and restart at the same time
  pthread_mutex_lock(&mutex_);

  // Reload configuration files
  Setting::Instance()->Reload();
  Acl::Instance()->Reload();

  // Reload settings from configuration files
  xmpp_->Init();

  pthread_mutex_unlock(&mutex_);
}

void Node::Stop() {
  xmpp_->Stop();
}

bool Node::Valid() {

  // Validate the settings file
  NodeSettingValidator setting = NodeSettingValidator(config_path_);
  setting.Validate();
  if (setting.HasError()) {
    LOG(ERROR) << "Unable to load settings file.";
  } else if (setting.IsFatal()) {
    setting.ReportIssues();
    return false;
  }

  // Validate acl file
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

  Setting::Instance()->OpenFile(config_path_);
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

  Acl::Instance()->OpenFile(Setting::Instance()->Get("general", "acl"));
  if (Acl::Instance()->HasError()) {
    std::cerr << "Unable to load ACL file." << std::endl;
    return false;
  }

  std::string log_path = Setting::Instance()->Get("general", "log");
  Logging *logging = Logging::Instance();
  if (debug_) {
    logging->set_level(DEBUG);
  } else if (!log_path.empty() && !logging->OpenFile(log_path)) {
    std::cerr << "Unable to open log file." << std::endl;
    return false;
  } else {
    logging->set_stderr(false);
  }

  return true;
}

int Node::Run() {
  if (!xmpp_) xmpp_ = new Xmpp();


  bool reconnect = true;

  while (reconnect) {

    // We need to wait here if we're reloading
    while (xmpp_->state() == Xmpp::Reload) usleep(100000);

    pthread_mutex_lock(&mutex_);

    // Sleep for a while before we try to reconnect if we didn't explicitly
    // disconnect.
    reconnect = xmpp_->state() != Xmpp::Shutdown;

    if (reconnect) {
      if (xmpp_->state() != Xmpp::None) {
        LOG(INFO) << "Reconnecting in " << NODE_RECONNECT << " seconds...";
        sleep(NODE_RECONNECT);
      }
    } else {
      pthread_mutex_unlock(&mutex_);
      break;
    }

    // Try to connect to XMPP server and handle events
    xmpp_->Connect();

    pthread_mutex_unlock(&mutex_);
     
  }

  if (xmpp_->state() == Xmpp::Shutdown) {
    return 0;
  } else if (xmpp_->state() == Xmpp::Disconnected) {
    return 76;
  }

  return 1;
}

namespace signal {

void *SignalHandler(void *arg) {
  int sig;
  sigset_t set;

  // We want to watch for SIGHUP
  sigemptyset(&set);
  sigaddset(&set, SIGHUP);
  sigaddset(&set, SIGINT);
  sigaddset(&set, SIGTERM);
  pthread_sigmask(SIG_BLOCK, &set, NULL);

  while (true) {
    // Block until we get a SIGHUP
    sigwait(&set, &sig);

    switch (sig) {
      case SIGHUP:
        Node::Instance()->Reload();
        break;
      case SIGINT:
      case SIGTERM:
        Node::Instance()->Stop();
        pthread_exit(NULL);
        break;
    }
  }
}

}  // namespace tyrion::node::signal

} }  // namespace tyrion::node
