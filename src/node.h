/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef TYRION_NODE_H_
#define TYRION_NODE_H_

#include <csignal>
#include <pthread.h>
#include <string>
#include "node_xmpp.h"

namespace tyrion {
namespace node {

class Node {
  public:
    ~Node();

    inline static Node *Instance() {
      if (!instance_)
        instance_ = new Node;
      return instance_;
    }

    /**
     * Initial configuration setup.
     */
    bool Setup();

    /**
     * Connect to XMPP server and handle events.
     */
    int Run();

    /**
     * Try to reload settings in the safest way possible.
     */
    void Reload();

    /**
     * Shutdown and cleanup service.
     */
    void Stop();

    /**
     * Validate all settings, used before reloading.
     */
    bool Valid();

    /**
     * Display debug information.
     */
    inline bool debug() { return debug_; }
    inline void set_debug(bool debug) { debug_ = debug; }

    /**
     * Configuration file path.
     */
    inline std::string config_path() { return config_path_; }
    inline void set_config_path(std::string config_path) {
      config_path_ = config_path;
    }

  protected:
    Node();
    Node(Node const&) {};

  private:
    static Node* instance_;
    std::string config_path_;
    bool debug_;
    Xmpp *xmpp_;
    pthread_mutex_t mutex_;
    sigset_t set_;
};

namespace signal {

/**
 * Handle SIGHUP to reload settings.
 */
void *SignalHandler(void *arg);

}  // namespace tyrion::node::signal

} }  // namespace tyrion::node

#endif  // TYRION_NODE_H_
