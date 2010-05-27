/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include <cstring>
#include <pthread.h>
#include <iostream>
#include <sstream>
#include <string>
#include "client_service_queue.h"
#include "client_xmpp.h"
#include "logging.h"
#include "setting.h"
#include "utils.h"

void *start_xmpp(void *arg) {
  tyrion::client::Xmpp *xmpp=(tyrion::client::Xmpp*)arg;
  xmpp->Start();
  delete(xmpp);
}

int main(int argc, char* argv[]) {
  bool debug = false;
  int timeout = -1;
  std::string config_file;
  std::string jid;
  std::string service_type;

  for(int i = 1; i < argc; i++) {
    const char *option = argv[i];

    if (strcmp(option, "-c") == 0 || strcmp(option, "--config-file") == 0) {
      if (i + 1 < argc) {
        config_file = std::string(argv[i+1]);
      } else {
        std::cerr << "Configuration file not specified." << std::endl;
        return 1;
      }
      i++;
    } else if (strcmp(option, "-j") == 0 || strcmp(option, "--jid") == 0) {
      if (i + 1 < argc) {
        jid = std::string(argv[i+1]);
      } else {
        std::cerr << "JID not specified." << std::endl;
        return 1;
      }
      i++;
    } else if (strcmp(option, "-s") == 0 ||
             strcmp(option, "--service-type") == 0) {
      if (i + 1 < argc) {
        service_type = std::string(argv[i+1]);
      } else {
        std::cerr << "Service type not specified." << std::endl;
        return 1;
      }
      i++;
    } else if (strcmp(option, "-t") == 0 || strcmp(option, "--timeout") == 0) {
      if (i + 1 < argc) {
        std::istringstream timeout_stream(argv[i+1]);
        if (!(timeout_stream >> timeout))
          std::cerr << "Timeout must be a number." << std::endl;
      } else {
        std::cerr << "Service type not specified." << std::endl;
        return 1;
      }
      i++;
    } else if (strcmp(option, "--debug") == 0) {
      debug = true;
    } else if (strcmp(option, "--help") == 0) {
      std::cout << "Usage: tyrion [OPTION]..." << std::endl;
      std::cout << "Example: tyrion -c client.conf" << std::endl;
      std::cout << std::endl;
      std::cout << "Configuration options:" << std::endl;
      std::cout << "  -c, --config-file         the node configuration file" << std::endl;
      std::cout << "  -j, --jid                 service destination JID" << std::endl;
      std::cout << "  -s, --service-type        service type" << std::endl;
      std::cout << "  -t, --timeout             service timeout" << std::endl;
      std::cout << "  --debug                   pipe debug info to stderr" << std::endl;
      return 0;
    } else {
      std::cerr << "Unknown option '" << option << "'." << std::endl;
      return 1;
    }
  }

  if (jid.empty()) {
    std::cerr << "A destination JID is required." << std::endl;
    return 1;
  } else if (jid.find("/") == std::string::npos) {
    std::cerr << "JID requires a resource (ex: user@host/resource)." << std::endl;
    return 1;
  }

  if (service_type.empty()) {
    std::cerr << "A service type is required." << std::endl;
    return 1;
  }

  tyrion::Setting::Instance()->File(config_file);

  if (tyrion::Setting::Instance()->HasError()) {
    std::cerr << "Can't load configuration file '"
              << config_file << "'." << std::endl;
    return 1;
  }

  tyrion::Logging *logging = tyrion::Logging::Instance();

  if (debug) {
    logging->Level(tyrion::DEBUG);
    logging->Stderr(true);
  }

  tyrion::client::ServiceQueue *request = new tyrion::client::ServiceQueue();
  tyrion::client::ServiceQueue *response = new tyrion::client::ServiceQueue();
  tyrion::client::Xmpp *xmpp = new tyrion::client::Xmpp(request, response);
  pthread_t handler;
  void *status;

  int rc = pthread_create(&handler, NULL, start_xmpp, (void *)xmpp);

  if (rc == 0) {
    tyrion::XmppService service;
    service.set_timeout(timeout);
    service.set_type(service_type);

    std::string data, x;
    while (!std::cin.eof()) {
      std::getline(std::cin, x);
      data += x + '\n';
    }
    service.set_input(data);

    tyrion::client::ServiceQueueItem item(jid, service);
    item.set_notification(tyrion::client::ServiceQueueItem::Disconnect);
    request->Push(item);
    item = response->Pop();

    if (item.state() == tyrion::client::ServiceQueueItem::ServiceUnavailable) {
      std::cerr << tyrion::utils::Error("xmpp.service-unavailable",
          "The requested service isn't available.");
      return 1;
    } else {
      std::cout << item.service().output();
      std::cerr << item.service().error();
      return item.service().code();
    }
  } else {
    std::cerr << "Unable to create thread." << std::endl;
    return 1;
  }

  pthread_join(handler, NULL);
  delete(request);
  delete(response);

  return 0;
}
