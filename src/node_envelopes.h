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

#ifndef _TYRION_NODE_ENVELOPES_H_
#define _TYRION_NODE_ENVELOPES_H_

#include <string>
#include <txmpp/jid.h>
#include <txmpp/xmlelement.h>
#include "constants.h"

namespace tyrion {

class ServiceEnvelope {
  public:
    ServiceEnvelope();
    ServiceEnvelope(const txmpp::XmlElement *stanza);
    ~ServiceEnvelope();

    bool ValidRequest();
    std::string Path();

    const txmpp::XmlElement* Response();

    inline txmpp::Jid jid() { return jid_; }
    inline void set_jid(const txmpp::Jid& jid) { jid_ = jid; }

    inline std::string id() { return id_; }
    inline void set_id(const std::string& id) { id_ = id; }

    inline std::string type() { return type_; }
    inline void set_type(const std::string& type) { type_ = type; }

    inline std::string input() { return input_; }
    inline void set_input(const std::string& input) { input_ = input; }

    inline std::string output() { return output_; }
    inline void set_output(const std::string& output) { output_ = output; }

    inline std::string error() { return error_; }
    inline void set_error(const std::string& error) { error_ = error; }

    inline std::string user() { return user_; }
    inline void set_user(const std::string& user) { user_ = user; }

    inline std::string group() { return group_; }
    inline void set_group(const std::string& group) { group_ = group; }

    inline int timeout() { return timeout_; }
    inline void set_timeout(int timeout) { timeout_ = timeout > 0 ? timeout : PROCESS_TIMEOUT; }

    inline int code() { return code_; }
    inline void set_code(int code) { code_ = code; }

    inline bool valid() { return valid_; }
    inline void set_valid(bool valid) { valid_ = valid; }

  private:
    txmpp::Jid jid_;
    std::string id_;
    std::string input_;
    std::string output_;
    std::string error_;
    std::string type_;
    std::string user_;
    std::string group_;
    int code_;
    int timeout_;
    bool valid_;
};

}  // tyrion

#endif  // _TYRION_NODE_STANZA_H_
