/*
 * Copyright (c) 2010, Silas Sewell
 * All rights reserved.
 *
 * This file is subject to the MIT License (see the LICENSE file).
 */

#ifndef _TYRION_ENVELOPE_H_
#define _TYRION_ENVELOPE_H_

#include <txmpp/jid.h>
#include "common.h"

namespace txmpp {

class XmlElement;

}  // namespace txmpp

namespace tyrion {

class Loop;

class Envelope {
  public:
    Envelope(Loop* loop);
    virtual ~Envelope() {}

    bool Check();
    int Retry();
    std::string Path();

    const txmpp::XmlElement* Response();
    bool Update(const txmpp::XmlElement *stanza);

    inline txmpp::Jid& jid() { return jid_; }
    inline void set_jid(const txmpp::Jid& jid) { jid_ = jid; }
    inline void set_jid(const std::string& jid) { jid_ = txmpp::Jid(jid); }

    inline std::string id() { return id_; }
    inline void set_id(const std::string& id) { id_ = id; }

    inline std::string iq_id() { return iq_id_; }
    inline void set_iq_id(const std::string& iq_id) { iq_id_ = iq_id; }

    inline std::string type() { return type_; }
    inline void set_type(const std::string& type) { type_ = type; }

    inline std::string input() { return input_; }
    inline void set_input(const std::string& input) { input_ = input; }

    inline std::string output() { return output_; }
    inline void set_output(const std::string& output) { output_ = output; }
    inline void append_output(const std::string& output) { output_ += output; }

    inline std::string error() { return error_; }
    inline void set_error(const std::string& error) { error_ = error; }
    inline void append_error(const std::string& error) { error_ += error; }

    inline std::string user() { return user_; }
    inline void set_user(const std::string& user) { user_ = user; }

    inline std::string group() { return group_; }
    inline void set_group(const std::string& group) { group_ = group; }

    inline int timeout() { return timeout_; }
    inline void set_timeout(int timeout) {
      timeout_ = timeout > 0 ? timeout : PROCESS_TIMEOUT;
    }

    inline int code() { return code_; }
    inline void set_code(int code) { code_ = code; }

    inline int retry() { return retry_; }
    inline void set_retry(int retry) { retry_ = retry; }

  private:
    Loop* loop_;
    txmpp::Jid jid_;
    std::string id_;
    std::string iq_id_;
    std::string input_;
    std::string output_;
    std::string error_;
    std::string type_;
    std::string user_;
    std::string group_;
    int code_;
    int timeout_;
    int retry_;
    DISALLOW_EVIL_CONSTRUCTORS(Envelope);
};

}  // tyrion

#endif  // _TYRION_ENVELOPE_H_
