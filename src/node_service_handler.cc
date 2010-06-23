/*
 * Tyrion
 * Copyright 2010, Silas Sewell
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "node_service_handler.h"

#include "node_loop.h"
#include "node_settings.h"
#include "process.h"
#include "utils.h"

namespace tyrion {

NodeServiceHandler::NodeServiceHandler(NodeServiceEnvelope* service) {
  service_ = service;
}

NodeServiceHandler::~NodeServiceHandler() {
}

void NodeServiceHandler::Run() {
  bool issue = false;
  std::string output;
  std::string error;

  std::string config = "service:" + service_->type();

  std::string user = NodeSettings::Instance()->Get(config, "user");
  bool user_lock = NodeSettings::Instance()->GetBool(config, "user_lock");

  std::string group = NodeSettings::Instance()->Get(config, "group");
  bool group_lock = NodeSettings::Instance()->GetBool(config, "group_lock");

  int timeout = NodeSettings::Instance()->GetInt(config, "timeout");
  bool timeout_lock = NodeSettings::Instance()->GetBool(config, "timeout_lock");

  if (!issue && user_lock) {
    if (user.empty()) {
      error += CreateError("user.lock",
                            "Unable to lock user because none set.");
      issue = true;
    }
  } else if (!issue && !service_->user().empty()) {
    user = service_->user();
  }

  if (!issue && group_lock) {
    if (group.empty()) {
      error += CreateError("group.lock",
          "Unable to lock group because none set.");
      issue = true;
    }
  } else if (!issue && !service_->group().empty())
    group = service_->group();

  if (!issue && timeout_lock) {
    if (!timeout > 0) {
      error += CreateError("timeout.lock",
                            "Unable to lock timeout because none set.");
      issue = true;
    }
  } else {
    timeout = service_->timeout();
  }

  Process p = Process(service_->Path(), false, timeout);

  if (!issue && !user.empty() && !p.set_user(user)) {
    error += CreateError("user.lookup", "Unable to find user '" +
                          user + "'");
    issue = true;
  }

  if (!issue && !group.empty() && !p.set_group(group)) {
    error += CreateError("group.lookup", "Unable to find group '" +
                          group + "'");
    issue = true;
  }

  int code = -1;

  if (!issue) {
    p.Run();
    p.Write(service_->input());
    p.Eof();

    output += p.ReadAll(Process::Stdout);
    error += p.ReadAll(Process::Stderr);
    code = p.Close();
  }

  if (issue)
    service_->set_valid(false);

  service_->set_code(code);
  service_->set_output(output);
  service_->set_error(error);

  tyrion::NodeLoop::Instance()->Response(service_);
}

} // namespace tyrion
