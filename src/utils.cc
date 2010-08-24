/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include "utils.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <string>
#include "common.h"
#include "loop.h"
#include "service_handler.h"
#include "settings.h"
#include "utils.h"

namespace tyrion {

std::string CreateError(std::string code, std::string message) {
  Escape(message);
  return "ERROR: org.tyrion.error." + code + (!message.empty() ? ": " +
      message : message) + "\n";
}

void Escape(std::string& text) {
  StringReplace(text, "\n", "\\n");
  StringReplace(text, "\r", "\\r");
}

void Exit(int code) {
  TLOG(DEBUG) << "Exiting...";
  delete Logging::Instance();
  exit(code >= 0 ? code : 0);
}

std::string RealPath(const std::string& path) {
  char buffer[PATH_MAX + 1];
  char *rc = realpath(path.c_str(), buffer);

  return rc ? std::string(buffer) : "";
}

void StringReplace(std::string& text, const std::string& search,
                   const std::string& replace) {
  std::string::size_type pos = text.find(search);

  while (pos != std::string::npos) {
    text.replace(pos, search.length(), replace);
    pos = text.find(search, pos+search.length());
  }
}

Loop* Reload(int argc, char* argv[], Loop* old_loop,
                     ServiceHandler* service_handler) {
  old_loop->SetReconnect(false);

  Loop* new_loop = new Loop(pthread_self());

  int code = tyrion::Setup(argc, argv, new_loop, true);

  if (code != 0 ) {
    delete new_loop;
    TLOG(ERROR) << "Unable to reload Tyrion (invalid settings).";
    return old_loop;
  }

  new_loop->Start();
  new_loop->SetReconnect(false);
  new_loop->Login();

  for (int i = 0; i < 10; i++) {
    if (new_loop->Ready() && !old_loop->Ready()) {
      new_loop->set_service_handler(service_handler);
      new_loop->SetReconnect(true);

      old_loop->Quit();
      old_loop->Stop();
      delete old_loop;

      TLOG(INFO) << "Tyrion reloaded.";

      return new_loop;
    } else if (new_loop->state() == Loop::ERROR) {
      break;
    }
    sleep(1);
  }

  new_loop->Quit();
  new_loop->Stop();

  delete new_loop;

  old_loop->SetReconnect(true);

  if (!old_loop->Ready())
    old_loop->Login();

  TLOG(ERROR) << "Unable to reload Tyrion (connection failed).";

  return old_loop;
}

bool SetupLogging(Settings* settings, bool reload) {
  Logging* logging = NULL;
  if (reload) {
    logging = Logging::New();
  } else {
    logging = Logging::Instance();
  }
  Logging* old_logging = NULL;

  Logging::Level log_level = Logging::StringToLevel(
      settings->Get(SETTING_GENERAL, SETTING_LOG_LEVEL), Logging::INFO);
  std::string log_path = settings->Get(SETTING_GENERAL, SETTING_LOG_PATH);

  if (!logging->File(log_path, log_level)) {
    TLOG(ERROR) << "Unable to open log file.";
    if (reload)
      delete logging;
    return false;
  }

  if (reload) {
    old_logging = Logging::Instance();
    logging->Debug(old_logging->debug_level());
  }

  // Replace logging instance
  Logging::Instance(logging);

  if (reload)
    delete old_logging;

  return true;
}

int Setup(int argc, char* argv[], Loop* loop, bool reload) {
  const char *config = NULL;
  bool debug = false;

  for(int i = 1; i < argc; i++) {
    const char* option = argv[i];

    if (OPTION("-c") || OPTION("--config-file")) {
      if (i + 1 < argc) {
        config = argv[++i];
      } else {
        TLOG(ERROR) << "Configuration file not specified.";
        return 1;
      }
    } else if (OPTION("--debug")) {
      debug = true;
    } else if (OPTION("--help")) {
      std::cout << "Usage: tyrion [OPTION]..." << std::endl;
      std::cout << "Example: tyrion -c tyrion.conf" << std::endl;
      std::cout << std::endl;
      std::cout << "Configuration options:" << std::endl;
      std::cout << "  -c, --config-file         configuration file"
                << std::endl;
      std::cout << std::endl;
      std::cout << "Misc options:" << std::endl;
      std::cout << "  --debug                   debug information"
                << std::endl;
      return -1;
    } else {
      TLOG(ERROR) << "Unknown option '" << option << "'.";
      return 1;
    }
  }

  if (debug) Logging::Instance()->Debug(Logging::DEBUG);

  if (config == NULL || strlen(config) < 1) {
    TLOG(ERROR) << "Configuration file required.";
    return 1;
  }

  Settings* settings = new Settings(config);
  loop->set_settings(settings);

  if (settings->HasError()) {
    TLOG(ERROR) << "Unable to open configuration file.";
    return 1;
  }

  if (!settings->Validate()) {
    TLOG(ERROR) << "Invalid configuration file.";
    return 1;
  }

  Acls* acls = new Acls(
      settings->Get(SETTING_GENERAL, SETTING_ACL_PATH));
  loop->set_acls(acls);

  if (acls->HasError()) {
    TLOG(ERROR) << "Unable to open acl file.";
    return 1;
  }

  if (!acls->Validate()) {
    TLOG(ERROR) << "Invalid acl file.";
    return 1;
  }

  if (!SetupLogging(settings, reload))
    return 1;

  if (debug) {
    Logging::Instance()->Debug(Logging::DEBUG);
  } else {
    Logging::Instance()->Debug(Logging::NONE);
  }

  return 0;
}

}  // namespace tyrion
