/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_CONSTANTS_H_
#define _TYRION_CONSTANTS_H_

#include <string>
#include <txmpp/qname.h>

namespace tyrion {

extern const std::string NS_SERVICE;

extern const txmpp::QName QN_SERVICE;

extern const txmpp::QName QN_TIMEOUT;
extern const txmpp::QName QN_USER;
extern const txmpp::QName QN_GROUP;
extern const txmpp::QName QN_INPUT;

extern const txmpp::QName QN_CODE;
extern const txmpp::QName QN_OUTPUT;
extern const txmpp::QName QN_ERROR;

extern const std::string STR_ACL_PATH;
extern const std::string STR_GENERAL;
extern const std::string STR_JID;
extern const std::string STR_LOG_LEVEL;
extern const std::string STR_LOG_PATH;
extern const std::string STR_PASSWORD;
extern const std::string STR_PORT;
extern const std::string STR_SERVICE_PATH;
extern const std::string STR_XMPP;

extern const int XMPP_PORT;
extern const int MAX_RECONNECT_TIMEOUT;

extern const int PROCESS_TIMEOUT;
extern const int PROCESS_BUFFER_SLEEP;
extern const std::string PROCESS_ISSUE_COMMAND;

}  // namespace tyrion

#endif  // _TYRION_CONSTANTS_H_
