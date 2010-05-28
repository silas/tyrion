/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "node_service_handler.h"

#include <gloox/error.h>
#include "process.h"
#include "setting.h"
#include "tyrion.h"
#include "utils.h"

namespace tyrion {
namespace node {

ServiceHandler::ServiceHandler(gloox::ClientBase* client, gloox::JID jid,
                               std::string id, std::string type,
                               std::string input, std::string path) {
  client_ = client;
  jid_ = jid;
  id_ = id;
  type_ = type;
  input_ = input;
  path_ = path;
  timeout_ = SERVICE_TIMEOUT;
}

void ServiceHandler::Run() {
  bool issue = false;
  std::string output;
  std::string error;

  std::string config = "service:" + type_;

  std::string user = Setting::Instance()->Get(config, "user");
  bool user_lock = Setting::Instance()->GetBool(config, "user_lock");

  std::string group = Setting::Instance()->Get(config, "group");
  bool group_lock = Setting::Instance()->GetBool(config, "group_lock");

  int timeout = Setting::Instance()->GetInt(config, "timeout");
  bool timeout_lock = Setting::Instance()->GetBool(config, "timeout_lock");

  if (!issue && user_lock) {
    if (user.empty()) {
      error += utils::Error("user.lock",
                            "Unable to lock user because none set.");
      issue = true;
    }
  } else if (!issue && !user_.empty()) {
    user = user_;
  }

  if (!issue && group_lock) {
    if (group.empty()) {
      error += utils::Error("group.lock",
          "Unable to lock group because none set.");
      issue = true;
    }
  } else if (!issue && !group_.empty())
    group = group_;

  if (!issue && timeout_lock) {
    if (!timeout > 0) {
      error += utils::Error("timeout.lock",
                            "Unable to lock timeout because none set.");
      issue = true;
    }
  } else if (!issue && timeout_ > 0) {
    timeout = timeout_;
  } else {
    timeout = SERVICE_TIMEOUT;
  }

  Process p = Process(path_, false, timeout);

  if (!issue && !user.empty() && !p.set_user(user)) {
    error += utils::Error("user.lookup", "Unable to find user '" +
                          user + "'");
    issue = true;
  }

  if (!issue && !group.empty() && !p.set_group(group)) {
    error += utils::Error("group.lookup", "Unable to find group '" +
                          group + "'");
    issue = true;
  }

  int code = -1;

  if (!issue) {
    p.Run();
    p.Write(input_);
    p.Eof();

    output += p.ReadAll(Process::Stdout);
    error += p.ReadAll(Process::Stderr);
    code = p.Close();
  }

  XmppService *service = new XmppService();
  service->set_output(output);
  service->set_error(error);
  service->set_type(type_);
  service->set_code(code);

  gloox::IQ re(gloox::IQ::Result, jid_, id_);
  re.addExtension(service);
  client_->send(re);
}

} } // namespace tyrion::node
