/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef TYRION_XMPP_SERVICE_H_
#define TYRION_XMPP_SERVICE_H_

#include <gloox/client.h>
#include <gloox/stanzaextension.h>
#include "tyrion.h"

namespace tyrion {

const int ExtXmppService = gloox::ExtUser+1;

class XmppService : public gloox::StanzaExtension {
  public:
    enum Format {
      None,
      Response,
      Request
    };

    XmppService();
    XmppService(const gloox::Tag* tag);
    virtual ~XmppService() {}

    virtual gloox::StanzaExtension* newInstance(const gloox::Tag* tag)
      const {
      return new XmppService(tag);
    }

    gloox::Tag* tag() const;
    virtual const std::string& filterString() const;
    virtual gloox::StanzaExtension* clone() const { return new XmppService(*this); }

    int code() const { return code_; }
    void set_code(int code) { code_ = code; set_format(Response); }

    std::string error() const { return error_; }
    void set_error(std::string error) { error_ = error; set_format(Response); }

    Format format() { return format_; }
    void set_format(Format format) { format_ = format; }

    std::string group() const { return group_; }
    void set_group(std::string group) { group_ = group; }

    std::string input() const { return input_; }
    void set_input(std::string input) { input_ = input; set_format(Request); }

    std::string output() const { return output_; }
    void set_output(std::string output) { output_ = output; set_format(Response); }

    int timeout() const { return timeout_; }
    void set_timeout(int timeout) { timeout_ = timeout; set_format(Request); }

    std::string type() const { return type_; }
    void set_type(std::string type) { type_ = type; }

    std::string user() const { return user_; }
    void set_user(std::string user) { user_ = user; }

  private:
    int code_;
    std::string error_;
    Format format_;
    std::string group_;
    std::string input_;
    std::string output_;
    int timeout_;
    std::string type_;
    std::string user_;
};

}  // namespace tyrion

#endif  // TYRION_XMPP_SERVICE_H_
