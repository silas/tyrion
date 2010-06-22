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

#include "constants.h"

#include <txmpp/constants.h>

namespace tyrion {

const std::string NS_SERVICE("http://tyrion.org/protocol/1.0/service");

const txmpp::QName QN_SERVICE(true, NS_SERVICE, "service");
const txmpp::QName QN_INPUT(true, NS_SERVICE, "input");
const txmpp::QName QN_TIMEOUT(true, txmpp::STR_EMPTY, "timeout");
const txmpp::QName QN_USER(true, txmpp::STR_EMPTY, "user");
const txmpp::QName QN_GROUP(true, txmpp::STR_EMPTY, "group");

const int PROCESS_TIMEOUT = 300; // secoonds
const int RECONNECT_TIMEOUT = 10000; // milliseconds
const int SERVICE_RETRY_TIMEOUT = 12000; // milliseconds

};  // namespace tyrion
