/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include "client_request.h"

#include <sstream>
#include "client_envelope.h"
#include "client_settings.h"
#include "logging.h"

#define PROFILE(var, str) if (var.empty()) \
  var = settings->Get("profile:" + profile_, str)

namespace tyrion {

ClientRequest::ClientRequest() :
    jid_(),
    profile_(txmpp::STR_EMPTY),
    timeout_(txmpp::STR_EMPTY),
    service_(txmpp::STR_EMPTY),
    user_(txmpp::STR_EMPTY),
    group_(txmpp::STR_EMPTY),
    input_(txmpp::STR_EMPTY) {
}

ClientRequest::~ClientRequest() {
}

bool ClientRequest::Setup(ClientSettings* settings) {
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

ClientEnvelope* ClientRequest::CreateClientEnvelope(
    const std::string& jid) {
  ClientEnvelope* envelope = new ClientEnvelope();

  envelope->set_jid(jid);
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
  size_t begin, end;
  while(std::getline(ss, item, delimiter)) {
    begin = item.find_first_not_of(" \t");
    end = item.find_last_not_of(" \t");
    if((begin != std::string::npos) && (end != std::string::npos)) {
      item = item.substr(begin, end-begin+1);
      jid_.push_back(item);
    }
  }
}

}  // namespace tyrion
