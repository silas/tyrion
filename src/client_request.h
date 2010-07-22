/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_CLIENT_REQUEST_H_
#define _TYRION_CLIENT_REQUEST_H_

#include <vector>
#include "common.h"

namespace tyrion {

class ClientEnvelope;
class ClientSettings;

typedef std::vector<std::string> StringList;

class ClientRequest {
  public:
    ClientRequest();
    ~ClientRequest();

    bool Setup(ClientSettings* settings);

    ClientEnvelope* CreateClientEnvelope(const std::string& jid);

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

    inline std::string input() { return input_; }
    inline void set_input(const std::string& input) { input_ = input; }

  private:
    void Split(const std::string &text, char delimiter);
    StringList jid_;
    std::string profile_;
    std::string timeout_;
    std::string service_;
    std::string user_;
    std::string group_;
    std::string input_;
    DISALLOW_EVIL_CONSTRUCTORS(ClientRequest);
};

}  // namespace tyrion

#endif  // _TYRION_CLIENT_REQUEST_H_
