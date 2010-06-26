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

#ifndef _TYRION_CLIENT_REQUEST_H_
#define _TYRION_CLIENT_REQUEST_H_

#include <string>
#include <vector>
#include "client_envelope.h"

namespace tyrion {

typedef std::vector<std::string> StringList;

class ClientRequest {
  public:
    ClientRequest() {}
    ~ClientRequest() {}

    bool Setup();

    ClientServiceEnvelope* CreateClientServiceEnvelope(std::string jid);

    inline StringList* jid() { return &jid_; }
    inline void set_jid(const std::string& jid) { Split(jid, ','); }

    inline std::string profile() { return profile_; }
    inline void set_profile(const std::string& profile) { profile_ = profile; }

    inline std::string timeout() { return timeout_; }
    inline void set_timeout(std::string timeout) { timeout_ = timeout; }

    inline std::string service() { return service_; }
    inline void set_service(const std::string& service) { service_ = service; }

    inline std::string user() { return user_; }
    inline void set_user(const std::string& user) { user_ = user; }

    inline std::string group() { return group_; }
    inline void set_group(const std::string& group) { group_ = group; }

  private:
    void Split(const std::string &text, char delimiter);
    StringList jid_;
    std::string profile_;
    std::string timeout_;
    std::string service_;
    std::string user_;
    std::string group_;
    std::string input_;
};

};  // namespace tyrion

#endif  // _TYRION_CLIENT_REQUEST_H_
