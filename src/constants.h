/*
 * Copyright (c) 2010, Silas Sewell
 * All rights reserved.
 *
 * This file is subject to the MIT License (see the LICENSE file).
 */

#ifndef _TYRION_CONSTANTS_H_
#define _TYRION_CONSTANTS_H_

#include <string>
#include <txmpp/constants.h>
#include <txmpp/qname.h>

namespace tyrion {

extern const std::string NS_SERVICE;

extern const txmpp::QName QN_SERVICE;

extern const txmpp::QName QN_TIMEOUT;
extern const txmpp::QName QN_USER;
extern const txmpp::QName QN_GROUP;
extern const txmpp::QName QN_INPUT;

extern const txmpp::QName QN_SERVICE_ID;
extern const txmpp::QName QN_CODE;
extern const txmpp::QName QN_OUTPUT;
extern const txmpp::QName QN_ERROR;

#define SETTING_ACL_PATH "acl_path"
#define SETTING_GENERAL "general"
#define SETTING_JID "jid"
#define SETTING_LOG_LEVEL "log_level"
#define SETTING_LOG_PATH "log_path"
#define SETTING_PASSWORD "password"
#define SETTING_PORT "port"
#define SETTING_SERVER "server"
#define SETTING_SERVICE_PATH "service_path"
#define SETTING_XMPP "xmpp"

#define XMPP_PORT 5222
#define MAX_RECONNECT_TIMEOUT 256

#define PROCESS_TIMEOUT 300
#define PROCESS_POLL_TIMEOUT 500
#define PROCESS_BUFFER 1024
#define PROCESS_BUFFER_SLEEP 1
#define PROCESS_ISSUE_COMMAND "false"

}  // namespace tyrion

#endif  // _TYRION_CONSTANTS_H_
