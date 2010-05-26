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
#include "setting.h"

void *start_xmpp(void *arg)
{
  tyrion::client::Xmpp *xmpp=(tyrion::client::Xmpp*)arg;
  xmpp->Start();
  delete(xmpp);
}

int main(int argc, char* argv[])
{
  std::string config_file;
  std::string jid;
  std::string service_type;
  int timeout = 0;

  for(int i = 1; i < argc; i++)
  {
    const char *option = argv[i];

    if (strcmp(option, "-c") == 0 || strcmp(option, "--config-file") == 0)
    {
      if (i + 1 < argc)
        config_file = std::string(argv[i+1]);
      else
      {
        std::cerr << "Configuration file not specified." << std::endl;
        return 1;
      }
      i++;
    }
    else if (strcmp(option, "-j") == 0 || strcmp(option, "--jid") == 0)
    {
      if (i + 1 < argc)
        jid = std::string(argv[i+1]);
      else
      {
        std::cerr << "JID not specified." << std::endl;
        return 1;
      }
      i++;
    }
    else if (strcmp(option, "-s") == 0 || strcmp(option, "--service-type") == 0)
    {
      if (i + 1 < argc)
        service_type = std::string(argv[i+1]);
      else
      {
        std::cerr << "Service type not specified." << std::endl;
        return 1;
      }
      i++;
    }
    else if (strcmp(option, "-t") == 0 || strcmp(option, "--timeout") == 0)
    {
      if (i + 1 < argc)
      {
        std::istringstream timeout_stream(argv[i+1]);
        if (!(timeout_stream >> timeout))
          std::cerr << "Timeout must be a number." << std::endl;
      }
      else
      {
        std::cerr << "Service type not specified." << std::endl;
        return 1;
      }
      i++;
    }
    else if (strcmp(option, "--help") == 0)
    {
      std::cout << "Usage: tyrion [OPTION]..." << std::endl;
      std::cout << "Example: tyrion -c client.conf" << std::endl;
      std::cout << std::endl;
      std::cout << "Configuration options:" << std::endl;
      std::cout << "  -c, --config-file         the node configuration file" << std::endl;
      std::cout << "  -j, --jid                 service destination JID" << std::endl;
      std::cout << "  -s, --service-type        service type" << std::endl;
      std::cout << "  -t, --timeout             service timeout" << std::endl;
      return 0;
    }
    else
    {
      std::cerr << "Unknown option '" << option << "'." << std::endl;
      return 1;
    }
  }

  if (jid.empty())
  {
    std::cerr << "A destination JID is required." << std::endl;
    return 1;
  }

  if (service_type.empty())
  {
    std::cerr << "A service type is required." << std::endl;
    return 1;
  }

  tyrion::Setting::Instance()->File(config_file);

  if (tyrion::Setting::Instance()->HasError())
  {
    std::cerr << "Can't load configuration file '" << config_file << "'." << std::endl;
    return 1;
  }

  tyrion::client::ServiceQueue *request = new tyrion::client::ServiceQueue();
  tyrion::client::ServiceQueue *response = new tyrion::client::ServiceQueue();
  tyrion::client::Xmpp *xmpp = new tyrion::client::Xmpp(request, response);
  pthread_t handler;
  void *status;

  int rc = pthread_create(&handler, NULL, start_xmpp, (void *)xmpp);

  if (rc == 0)
  {
    tyrion::XmppService service;
    service.SetTimeout(timeout);
    service.SetType(service_type);

    std::string data, x;
    while (!std::cin.eof())
    {
      getline(std::cin, x);
      data += x + '\n';
    }
    service.SetInput(data);

    tyrion::client::ServiceQueueItem item(jid, service);
    item.SetNotification(tyrion::client::Disconnect);
    request->Push(item);
    item = response->Pop();

    std::cout << item.GetService().GetOutput();
    std::cerr << item.GetService().GetError();
  }
  else
  {
    std::cerr << "Unable to create thread." << std::endl;
    return 1;
  }

  pthread_join(handler, NULL);
  delete(request);
  delete(response);

  return 0;
}
