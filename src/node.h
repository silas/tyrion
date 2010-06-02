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
    static Node *Instance();
    bool Setup();
    void Run();
    void Reload();
    bool Valid();

    inline bool debug() { return debug_; }
    inline void set_debug(bool debug) { debug_ = debug; }

    inline std::string config_path() { return config_path_; }
    inline void set_config_path(std::string config_path) { config_path_ = config_path; }

  protected:
    Node();
    Node(Node const&) {};
    Node& operator=(Node const&) {};

  private:
    static Node* instance_;
    pthread_t handler_;
    std::string config_path_;
    bool debug_;
    pthread_mutex_t mutex_;
    sigset_t set_;
    Xmpp *xmpp_;
};

namespace signal {

void *SignalHandler(void *arg);

}  // namespace tyrion::node::signal

} }  // namespace tyrion::node

#endif  // TYRION_NODE_H_
