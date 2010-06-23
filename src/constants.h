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
extern const int PROCESS_TIMEOUT;
extern const int RECONNECT_TIMEOUT;
extern const int SERVICE_RETRY_TIMEOUT;

};  // namespace tyrion

#endif  // _TYRION_CONSTANTS_H_
