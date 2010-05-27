/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef TYRION_NODE_SERVICE_HANDLER_H_
#define TYRION_NODE_SERVICE_HANDLER_H_

#include <map>
#include <string>
#include "node_xmpp.h"

namespace tyrion {
namespace node {

class ServiceHandler {
  public:
    ServiceHandler(gloox::ClientBase* client, gloox::JID jid,
                   std::string id, std::string type, std::string input,
                   std::string path);
    ~ServiceHandler() {}

    void Run();

    std::string id() { return id_; }
    void set_id(std::string id) { id_ = id; }

    std::string group() { return group_; }
    void set_group(std::string group) { group_ = group; }

    std::string input() { return input_; }
    void set_input(std::string input) { input_ = input; }

    std::string path() { return path_; }
    void set_path(std::string path) { path_ = path; }

    int timeout() { return timeout_; }
    void set_timeout(int timeout) { timeout_ = timeout; }

    std::string type() { return type_; }
    void set_type(std::string type) { type_ = type; }

    std::string user() { return user_; }
    void set_user(std::string user) { user_ = user; }

  private:
    gloox::ClientBase* client_;
    std::string group_;
    std::string id_;
    std::string input_;
    gloox::JID jid_;
    std::string path_;
    int timeout_;
    std::string type_;
    std::string user_;
};

} }  // namespace tyrion::node

#endif  // TYRION_NODE_SERVICE_HANDLER_H_
