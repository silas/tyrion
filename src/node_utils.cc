/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include "node_utils.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include "constants.h"
#include "logging.h"
#include "node_loop.h"
#include "node_settings.h"
#include "utils.h"

namespace tyrion {

void NodeExit(int code) {
  TLOG(DEBUG) << "Exiting...";
  delete Logging::Instance();
  exit(code);
}

bool NodeReload() {
/*
  std::string config_path = NodeSettings::Instance()->path();

  NodeSettings* settings = NodeSettings::New();

  if (!settings->Setup(config_path)) {
    TLOG(ERROR) << "Unable to open configuration file.";
    delete settings;
    return false;
  }

  if (!settings->Validate()) {
    TLOG(ERROR) << "Invalid configuration file.";
    delete settings;
    return false;
  }

  std::string acl_path = NodeSettings::Instance()->Get(SETTING_GENERAL, SETTING_ACL_PATH);

  NodeAcls* acls = NodeAcls::New();

  if (!acls->Setup(acl_path)) {
    TLOG(ERROR) << "Unable to open acl file.";
    delete settings;
    delete acls;
    return false;
  }

  Logging* logging = Logging::New();

  Logging::Level log_level = Logging::StringToLevel(
      settings->Get(SETTING_GENERAL, SETTING_LOG_LEVEL), Logging::INFO);
  std::string log_path = settings->Get(SETTING_GENERAL, SETTING_LOG_PATH);
  if (!logging->File(log_path, log_level)) {
    TLOG(ERROR) << "Unable to open log file.";
    delete settings;
    delete acls;
    delete logging;
    return false;
  }

  NodeSettings* old_settings =  NodeSettings::Instance();
  NodeAcls* old_acls =  NodeAcls::Instance();
  Logging* old_logging =  Logging::Instance();

  logging->Debug(old_logging->debug_level());

  NodeSettings::Instance(settings);
  NodeAcls::Instance(acls);
  Logging::Instance(logging);

  delete old_settings;
  delete old_acls;
  delete old_logging;
*/

  return true;
}

bool NodeReloadLogging() {
/*
  Logging* logging = Logging::New();

  Logging::Level log_level = Logging::StringToLevel(
      NodeSettings::Instance()->Get(SETTING_GENERAL, SETTING_LOG_LEVEL),
      Logging::INFO);
  std::string log_path = NodeSettings::Instance()->Get(SETTING_GENERAL,
                                                       SETTING_LOG_PATH);
  if (!logging->File(log_path, log_level)) {
    TLOG(ERROR) << "Unable to open log file.";
    TLOG(ERROR) << "Unable to reload logging.";
    delete logging;
    return false;
  }

  Logging* old_logging =  Logging::Instance();

  logging->Debug(old_logging->debug_level());

  Logging::Instance(logging);

  delete old_logging;

  TLOG(INFO) << "Logging reloaded.";
*/

  return true;
}

NodeLoop* NodeSetup(int argc, char* argv[]) {
  NodeLoop* loop = new NodeLoop(pthread_self());

  const char *config = NULL;
  bool debug = false;

  for(int i = 1; i < argc; i++) {
    const char* option = argv[i];

    if (OPTION("-c") || OPTION("--config-file")) {
      if (i + 1 < argc) {
        config = argv[++i];
      } else {
        TLOG(ERROR) << "Configuration file not specified.";
        NodeExit(1);
      }
    } else if (OPTION("--debug")) {
      debug = true;
    } else if (OPTION("--help")) {
      std::cout << "Usage: tyrion-node [OPTION]..." << std::endl;
      std::cout << "Example: tyrion-node -c node.conf" << std::endl;
      std::cout << std::endl;
      std::cout << "Configuration options:" << std::endl;
      std::cout << "  -c, --config-file         the node configuration file"
                << std::endl;
      std::cout << "  --deubg                   enable debugging" << std::endl;
      NodeExit(0);
    } else {
      TLOG(ERROR) << "Unknown option '" << option << "'.";
      NodeExit(1);
    }
  }

  if (debug) Logging::Instance()->Debug(Logging::DEBUG);

  if (config == NULL || strlen(config) < 1) {
    TLOG(ERROR) << "Configuration file required.";
    NodeExit(1);
  }

  NodeSettings* settings = new NodeSettings(config);

  if (settings->HasError()) {
    TLOG(ERROR) << "Unable to open configuration file.";
    NodeExit(1);
  }

  if (!settings->Validate()) {
    TLOG(ERROR) << "Invalid configuration file.";
    NodeExit(1);
  }

  NodeAcls* acls = new NodeAcls(
      settings->Get(SETTING_GENERAL, SETTING_ACL_PATH));

  if (acls->HasError()) {
    TLOG(ERROR) << "Unable to open acl file.";
    NodeExit(1);
  }

  if (!acls->Validate()) {
    TLOG(ERROR) << "Invalid acl file.";
    NodeExit(1);
  }

  Logging::Level log_level = Logging::StringToLevel(
      settings->Get(SETTING_GENERAL, SETTING_LOG_LEVEL), Logging::INFO);
  std::string log_path = settings->Get(SETTING_GENERAL, SETTING_LOG_PATH);
  if (!Logging::Instance()->File(log_path, log_level)) {
    TLOG(ERROR) << "Unable to open log file.";
    NodeExit(1);
  } else if (!debug) {
    Logging::Instance()->Debug(Logging::NONE);
  }

  loop->set_settings(settings);
  loop->set_acls(acls);

  return loop;
}

}  // namespace tyrion
