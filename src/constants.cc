/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include "constants.h"

#include <txmpp/constants.h>

namespace tyrion {

const std::string NS_SERVICE("http://tyrion.org/protocol/1.0/service");

const txmpp::QName QN_SERVICE(true, NS_SERVICE, "service");

const txmpp::QName QN_TIMEOUT(true, txmpp::STR_EMPTY, "timeout");
const txmpp::QName QN_USER(true, txmpp::STR_EMPTY, "user");
const txmpp::QName QN_GROUP(true, txmpp::STR_EMPTY, "group");
const txmpp::QName QN_INPUT(true, NS_SERVICE, "input");

const txmpp::QName QN_CODE(true, txmpp::STR_EMPTY, "code");
const txmpp::QName QN_OUTPUT(true, NS_SERVICE, "output");
const txmpp::QName QN_ERROR(true, NS_SERVICE, "error");

const std::string STR_ACL_PATH = "acl_path";
const std::string STR_GENERAL = "general";
const std::string STR_JID = "jid";
const std::string STR_LOG_LEVEL = "log_level";
const std::string STR_LOG_PATH = "log_path";
const std::string STR_PASSWORD = "password";
const std::string STR_PORT = "port";
const std::string STR_SERVICE_PATH = "service_path";
const std::string STR_XMPP = "xmpp";

const int XMPP_PORT = 5222;
const int PROCESS_TIMEOUT = 300; // secoonds
const int MAX_RECONNECT_TIMEOUT = 256; // seconds

}  // namespace tyrion
