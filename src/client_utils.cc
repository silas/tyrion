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

#include "client_utils.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include "client_settings.h"

#define OPTION(str) strcmp(option, str) == 0

namespace tyrion {

void ClientExit(int code) {
  TLOG(DEBUG) << "Exiting...";
  delete Logging::Instance();
  delete ClientSettings::Instance();
  exit(code);
}

void ClientSetup(int argc, char* argv[], ClientRequest* request) {

  const char *config = NULL;
  bool debug = false;

  for(int i = 1; i < argc; i++) {
    const char* option = argv[i];

    if (OPTION("-c") || OPTION("--config-file")) {
      if (i + 1 < argc) {
        config = argv[++i];
      } else {
        TLOG(ERROR) << "Configuration file not specified.";
        ClientExit(1);
      }
    } else if (OPTION("-j") || OPTION("--jid")) {
      if (i + 1 < argc) {
        request->set_jid(argv[++i]);
      } else {
        std::cerr << "JID not specified." << std::endl;
        ClientExit(1);
      }
    } else if (OPTION("-s") || OPTION("--service")) {
      if (i + 1 < argc) {
        request->set_service(argv[++i]);
      } else {
        std::cerr << "Service type not specified." << std::endl;
        ClientExit(1);
      }
    } else if (OPTION("-t") || OPTION("--timeout")) {
      if (i + 1 < argc) {
        request->set_timeout(argv[++i]);
      } else {
        std::cerr << "Timeout type not specified." << std::endl;
        ClientExit(1);
      }
    } else if (OPTION("-u") || OPTION("--user")) {
      if (i + 1 < argc) {
        request->set_user(argv[++i]);
      } else {
        std::cerr << "User not specified." << std::endl;
        ClientExit(1);
      }
    } else if (OPTION("-g") || OPTION("--group")) {
      if (i + 1 < argc) {
        request->set_group(argv[++i]);
      } else {
        std::cerr << "Group not specified." << std::endl;
        ClientExit(1);
      }
    } else if (OPTION("-p") || OPTION("--profile")) {
      if (i + 1 < argc) {
        request->set_profile(argv[++i]);
      } else {
        std::cerr << "Profile not specified." << std::endl;
        ClientExit(1);
      }
    } else if (OPTION("--debug")) {
      debug = true;
    } else if (OPTION("--help")) {
      std::cout << "Usage: tyrion [OPTION]..." << std::endl;
      std::cout << "Example: tyrion -c client.conf" << std::endl;
      std::cout << std::endl;
      std::cout << "Configuration options:" << std::endl;
      std::cout << "  -c, --config-file         the node configuration file" << std::endl;
      std::cout << "  -p, --profile             default service options" << std::endl;
      std::cout << std::endl;
      std::cout << "Service options:" << std::endl;
      std::cout << "  -j, --jid                 destination JID(s)" << std::endl;
      std::cout << "  -s, --service             service type" << std::endl;
      std::cout << "  -t, --timeout             max service run time" << std::endl;
      std::cout << "  -u, --user                run service as user" << std::endl;
      std::cout << "  -g, --group               run service as group" << std::endl;
      std::cout << std::endl;
      std::cout << "Misc options:" << std::endl;
      std::cout << "  --debug                   show debug information" << std::endl;
      ClientExit(0);
    } else {
      TLOG(ERROR) << "Unknown option '" << option << "'.";
      ClientExit(1);
    }
  }

  if (debug) Logging::Instance()->Debug(Logging::DEBUG);

  if (config == NULL || strlen(config) < 1) {
    TLOG(ERROR) << "Configuration file required.";
    ClientExit(1);
  }

  if (!ClientSettings::Instance()->Setup(config)) {
    TLOG(ERROR) << "Unable to open configuration file.";
    ClientExit(1);
  }

  if (!ClientSettings::Instance()->Validate()) {
    TLOG(ERROR) << "Invalid configuration file.";
    ClientExit(1);
  }

  if (!request->Setup()) {
    TLOG(ERROR) << "Invalid request.";
    ClientExit(1);
  }
}

};  // namespace tyrion
