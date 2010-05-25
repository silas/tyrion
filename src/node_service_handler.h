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

namespace tyrion
{
  namespace node
  {

    class ServiceHandler
    {
      public:
        ServiceHandler(
          gloox::ClientBase* client,
          gloox::JID jid,
          std::string id,
          std::string type,
          std::string input,
          std::string path
        );
        ~ServiceHandler() {}
        void Run();
        void SetId(std::string id) { id_ = id; }
        void SetType(std::string type) { type_ = type; }
        void SetInput(std::string input) { input_ = input; }
        void SetPath(std::string path) { path_ = path; }
        void SetUser(std::string user) { user_ = user; }
        void SetGroup(std::string group) { group_ = group; }
        void SetTimeout(int timeout) { timeout_ = timeout; }
        std::string GetId() { return id_; }
        std::string GetType() { return type_; }
        std::string GetInput() { return input_; }
        std::string GetPath() { return path_; }
        std::string GetUser() { return user_; }
        std::string GetGroup() { return group_; }
        int GetTimeout() { return timeout_; }

      private:
        gloox::ClientBase* client_;
        gloox::JID jid_;
        std::string id_;
        std::string type_;
        std::string input_;
        std::string path_;
        std::string user_;
        std::string group_;
        int timeout_;
    };

  }
}

#endif
