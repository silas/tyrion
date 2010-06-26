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

#include "client_request.h"
#include "client_settings.h"
#include "logging.h"

#include <sstream>

#define PROFILE(var, str) if (var.empty()) \
  var = settings->Get("profile:" + profile_, str)

namespace tyrion {

bool ClientRequest::Setup() {
  ClientSettings* settings = ClientSettings::Instance();

  bool valid = true;

  if (!profile_.empty()) {
    if (jid_.empty()) set_jid(settings->Get("profile:" + profile_, "jid"));
    PROFILE(timeout_, "timeout");
    PROFILE(service_, "service");
    PROFILE(user_, "user");
    PROFILE(group_, "group");
  }

  if (jid_.empty()) {
    valid = false;
    TLOG(ERROR) << "The 'jid' option is required.";
  }

  if (service_.empty()) {
    valid = false;
    TLOG(ERROR) << "The 'service' option is required.";
  }

  if (!timeout_.empty()) {
    int number;
    std::istringstream number_stream(timeout_);
    if (!(number_stream >> number)) TLOG(ERROR) << "Timeout must be a number.";
  }

  return valid;
}

ClientServiceEnvelope* ClientRequest::CreateClientServiceEnvelope(
    std::string jid) {
  ClientServiceEnvelope* envelope = new ClientServiceEnvelope();

  envelope->set_jid(txmpp::Jid(jid));
  envelope->set_type(service_);
  if (!timeout_.empty()) {
    int number;
    std::istringstream number_stream(timeout_);
    if (number_stream >> number) {
      envelope->set_timeout(number);
    } else {
      TLOG(WARNING) << "Unable to convert timeout to number.";
    }
  }
  envelope->set_user(user_);
  envelope->set_group(group_);
  envelope->set_input(input_);

  return envelope;
}

void ClientRequest::Split(const std::string &text, char delimiter) {
  std::stringstream ss(text);
  std::string item;
  while(std::getline(ss, item, delimiter)) {
    jid_.push_back(item);
  }
}

};  // namespace tyrion
