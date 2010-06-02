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

    /**
     * Create and run the service process.
     */
    void Run();

    /**
     * The service type or name (ex: org.tyrion.service.bash).
     */
    std::string type() { return type_; }
    void set_type(std::string type) { type_ = type; }

    /**
     * IQ id for service message (we need to include it in the response).
     */
    std::string id() { return id_; }
    void set_id(std::string id) { id_ = id; }

    /**
     * Absolute path to service executable.
     */
    std::string path() { return path_; }
    void set_path(std::string path) { path_ = path; }

    /**
     * Input text which will be sent to stdin after creating the service
     * process.
     */
    std::string input() { return input_; }
    void set_input(std::string input) { input_ = input; }

    /**
     * Maximum time (in seconds) the service is allowed to run.
     */
    int timeout() { return timeout_; }
    void set_timeout(int timeout) { timeout_ = timeout; }

    /**
     * Group name to run the service as.
     */
    std::string group() { return group_; }
    void set_group(std::string group) { group_ = group; }

    /**
     * User name to run the service as.
     */
    inline std::string user() { return user_; }
    inline void set_user(std::string user) { user_ = user; }

  private:
    gloox::ClientBase* client_;
    gloox::JID jid_;
    std::string id_;
    std::string type_;
    std::string input_;
    int timeout_;
    std::string path_;
    std::string user_;
    std::string group_;
};

} }  // namespace tyrion::node

#endif  // TYRION_NODE_SERVICE_HANDLER_H_
