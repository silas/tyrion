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
  xmpp->Connect();
  delete(xmpp);
  pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
  bool debug = false;
  int timeout = -1;
  std::string config_file;
  std::string jid;
  std::string service_type;
  std::string profile;
  std::string user;
  std::string group;

  for(int i = 1; i < argc; i++) {
    const char *option = argv[i];

    if OPTION("-c", "--config-file") {
      if (i + 1 < argc) {
        config_file = std::string(argv[++i]);
      } else {
        std::cerr << "Configuration file not specified." << std::endl;
        return 1;
      }
    } else if OPTION("-j", "--jid") {
      if (i + 1 < argc) {
        jid = std::string(argv[++i]);
      } else {
        std::cerr << "JID not specified." << std::endl;
        return 1;
      }
    } else if OPTION("-s", "--service") {
      if (i + 1 < argc) {
        service_type = std::string(argv[++i]);
      } else {
        std::cerr << "Service type not specified." << std::endl;
        return 1;
      }
    } else if OPTION("-t", "--timeout") {
      if (i + 1 < argc) {
        std::istringstream timeout_stream(argv[++i]);
        if (!(timeout_stream >> timeout))
          std::cerr << "Timeout must be a number." << std::endl;
      } else {
        std::cerr << "Service type not specified." << std::endl;
        return 1;
      }
    } else if OPTION("-u", "--user") {
      if (i + 1 < argc) {
        user = std::string(argv[++i]);
      } else {
        std::cerr << "User not specified." << std::endl;
        return 1;
      }
    } else if OPTION("-g", "--group") {
      if (i + 1 < argc) {
        group = std::string(argv[++i]);
      } else {
        std::cerr << "Group not specified." << std::endl;
        return 1;
      }
    } else if OPTION("-p", "--profile") {
      if (i + 1 < argc) {
        profile = std::string(argv[++i]);
      } else {
        std::cerr << "Profile not specified." << std::endl;
        return 1;
      }
    } else if (strcmp(option, "--debug") == 0) {
      debug = true;
    } else if (strcmp(option, "--help") == 0) {
      std::cout << "Usage: tyrion [OPTION]..." << std::endl;
      std::cout << "Example: tyrion -c client.conf" << std::endl;
      std::cout << std::endl;
      std::cout << "Configuration options:" << std::endl;
      std::cout << "  -c, --config-file         the node configuration file" << std::endl;
      std::cout << "  -p, --profile             default service options" << std::endl;
      std::cout << std::endl;
      std::cout << "Service options:" << std::endl;
      std::cout << "  -j, --jid                 destination JID" << std::endl;
      std::cout << "  -s, --service             service type" << std::endl;
      std::cout << "  -t, --timeout             max service run time" << std::endl;
      std::cout << "  -u, --user                run service as user" << std::endl;
      std::cout << "  -g, --group               run service as group" << std::endl;
      std::cout << std::endl;
      std::cout << "Misc options:" << std::endl;
      std::cout << "  --debug                   show debug information" << std::endl;
      return 0;
    } else {
      std::cerr << "Unknown option '" << option << "'." << std::endl;
      return 1;
    }
  }

  tyrion::Setting *setting = tyrion::Setting::Instance();

  if (config_file.empty()) {
    std::cerr << "Configuration file required." << std::endl;
    return 1;
  }

  setting->OpenFile(config_file);

  if (setting->HasError()) {
    std::cerr << "Can't load configuration file '"
              << config_file << "'." << std::endl;
    return 1;
  }

  tyrion::Logging *logging = tyrion::Logging::Instance();

  if (debug) {
    logging->set_level(tyrion::DEBUG);
    logging->set_stderr(true);
  }

  if (!profile.empty()) {
    profile = "profile:" + profile;

    if (jid.empty())
      jid = setting->Get(profile, "jid");

    if (service_type.empty())
      service_type = setting->Get(profile, "service");

    if (timeout < 0)
      timeout = setting->GetInt(profile, "timeout", timeout);

    if (user.empty())
      user = setting->Get(profile, "user");

    if (group.empty())
      group = setting->Get(profile, "group");
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

  tyrion::client::ServiceQueue *request = new tyrion::client::ServiceQueue();
  tyrion::client::ServiceQueue *response = new tyrion::client::ServiceQueue();
  tyrion::client::Xmpp *xmpp = new tyrion::client::Xmpp(request, response);
  pthread_t handler;

  int rc = pthread_create(&handler, NULL, start_xmpp, (void *)xmpp);

  if (rc == 0) {
    tyrion::XmppService service;
    service.set_timeout(timeout);
    service.set_type(service_type);
    service.set_user(user);
    service.set_group(group);

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

  delete(request);
  delete(response);

  pthread_join(handler, NULL);

  return 0;
}
