#include <cstring>
#include <iostream>
#include <string>
#include "logging.h"
#include "node.h"
#include "setting.h"

int main(int argc, char* argv[]) {

  tyrion::Logging::Instance()->SetStderr(true);

  for(int i = 1; i < argc; i++) {
    const char* option = argv[i];

    if OPTION("-c", "--config-file") {
      if (i + 1 < argc) {
        tyrion::node::Node::Instance()->set_config_path(argv[++i]);
      } else {
        std::cerr << "Configuration file not specified." << std::endl;
        return 1;
      }
    } else if (strcmp(option, "--debug") == 0) {
      tyrion::node::Node::Instance()->set_debug(true);
    } else if (strcmp(option, "--help") == 0) {
      std::cout << "Usage: tyrion-node [OPTION]..." << std::endl;
      std::cout << "Example: tyrion-node -c node.conf" << std::endl;
      std::cout << std::endl;
      std::cout << "Configuration options:" << std::endl;
      std::cout << "  -c, --config-file         the node configuration file" << std::endl;
      std::cout << std::endl;
      std::cout << "Misc options:" << std::endl;
      std::cout << "  --debug                   show debug information" << std::endl;
      return 0;
    } else {
      std::cerr << "Unknown option '" << option << "'." << std::endl;
      return 1;
    }
  }

  if (!tyrion::node::Node::Instance()->Setup())
    return 1;

  tyrion::node::Node::Instance()->Run();

  return 0;
}
