/*
 * Copyright (c) 2010, Silas Sewell
 * All rights reserved.
 *
 * This file is subject to the MIT License (see the LICENSE file).
 */

#include "constants.h"

namespace tyrion {

const std::string NS_SERVICE("http://tyrion.org/protocol/1.0/service");

const txmpp::QName QN_SERVICE(true, NS_SERVICE, "service");

const txmpp::QName QN_TIMEOUT(true, txmpp::STR_EMPTY, "timeout");
const txmpp::QName QN_USER(true, txmpp::STR_EMPTY, "user");
const txmpp::QName QN_GROUP(true, txmpp::STR_EMPTY, "group");
const txmpp::QName QN_INPUT(true, NS_SERVICE, "input");

const txmpp::QName QN_SERVICE_ID(true, txmpp::STR_EMPTY, "id");
const txmpp::QName QN_CODE(true, txmpp::STR_EMPTY, "code");
const txmpp::QName QN_OUTPUT(true, NS_SERVICE, "output");
const txmpp::QName QN_ERROR(true, NS_SERVICE, "error");

}  // namespace tyrion
