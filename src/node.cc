/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include <iostream>
#include <string>

#include "logging.h"
#include "node_acl.h"
#include "node_xmpp.h"
#include "setting.h"
#include "tyrion.h"

int main(int argc, char* argv[])
{
  std::string config_file = tyrion::CONFIG_PATH;
  std::string acl_file;
  std::string log_file;
  bool debug = false;

  for(int i = 1; i < argc; i++)
  {
    const char* option = argv[i];

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
    else if (strcmp(option, "-a") == 0 || strcmp(option, "--acl-file") == 0)
    {
      if (i + 1 < argc)
        acl_file = std::string(argv[i+1]);
      else
      {
        std::cerr << "ACL file not specified." << std::endl;
        return 1;
      }
      i++;
    }
    else if (strcmp(option, "-l") == 0 || strcmp(option, "--log-file") == 0)
    {
      if (i + 1 < argc)
        log_file = std::string(argv[i+1]);
      else
      {
        std::cerr << "Log file not specified." << std::endl;
        return 1;
      }
      i++;
    }
    else if (strcmp(option, "--debug") == 0)
    {
      debug = true;
    }
    else if (strcmp(option, "--help") == 0)
    {
      std::cout << "Usage: tyrion-node [OPTION]..." << std::endl;
      std::cout << "Example: tyrion-node -c " << tyrion::CONFIG_PATH << std::endl;
      std::cout << std::endl;
      std::cout << "Configuration options:" << std::endl;
      std::cout << "  -c, --config-file         the node configuration file" << std::endl;
      std::cout << "  -a, --acl-file            the ACL configuration file" << std::endl;
      std::cout << "  -l, --log-file            the log file" << std::endl;
      std::cout << "  --debug                   pipe debug info to stderr" << std::endl;
      return 0;
    }
    else
    {
      std::cerr << "Unknown option '" << option << "'." << std::endl;
      return 1;
    }
  }

  tyrion::Setting::Instance()->File(config_file);

  if (tyrion::Setting::Instance()->HasError())
  {
    std::cerr << "Can't load configuration file '" << config_file << "'." << std::endl;
    return 1;
  }

  if (log_file.empty())
    log_file = tyrion::Setting::Instance()->Get("general", "log", tyrion::LOG_PATH);

  tyrion::Logging *logging = tyrion::Logging::Instance();

  if (debug)
  {
    logging->Level(tyrion::DEBUG);
    logging->Stderr(true);
  }
  else if (!log_file.empty() && !logging->File(log_file))
  {
    std::cerr << "Unable to open log file." << std::endl;
    return 0;
  }

  if (acl_file.empty())
    acl_file = tyrion::Setting::Instance()->Get("general", "acl", tyrion::ACL_PATH);

  tyrion::Config acl(acl_file);
  tyrion::node::Acl::Instance()->File(acl_file);

  if (tyrion::node::Acl::Instance()->HasError())
  {
    std::cerr << "Can't load ACL file '" << acl_file << "'." << std::endl;
    return 1;
  }

  if (tyrion::Setting::Instance()->Get("xmpp", "jid").empty())
  {
    std::cerr << "The xmpp jid field is required." << std::endl;
    return 1;
  }

  tyrion::node::Xmpp *xmpp = new tyrion::node::Xmpp();
  xmpp->Start();
  delete(xmpp);

  return 0;
}
