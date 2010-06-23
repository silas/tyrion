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

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include "node_settings.h"

#define OPTION(str) strcmp(option, str) == 0

namespace tyrion {

void NodeExit(int code) {
  delete Logging::Instance();
  delete NodeSettings::Instance();
  delete NodeAcls::Instance();
  exit(code);
}

void NodeSetupConfig(int argc, char* argv[]) {

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

  if (!NodeSettings::Instance()->Setup(config)) {
    TLOG(ERROR) << "Unable to open settings file.";
    NodeExit(1);
  }

  if (!NodeSettings::Instance()->Validate()) {
    TLOG(ERROR) << "Invalid settings.";
    NodeExit(1);
  }

  std::string acl_path = NodeSettings::Instance()->Get(STR_GENERAL, STR_ACL_PATH);
  if (!NodeAcls::Instance()->Setup(acl_path)) {
    TLOG(ERROR) << "Unable to open acl file.";
    NodeExit(1);
  }

  Logging::Level log_level = Logging::StringToLevel(
      NodeSettings::Instance()->Get(STR_GENERAL, STR_LOG_LEVEL), Logging::INFO);
  std::string log_path = NodeSettings::Instance()->Get(STR_GENERAL, STR_LOG_PATH);
  if (!Logging::Instance()->File(log_path, log_level)) {
    TLOG(ERROR) << "Unable to open log file.";
    NodeExit(1);
  } else if (!debug) {
    Logging::Instance()->Debug(Logging::NONE);
  }
}

};  // namespace tyrion
