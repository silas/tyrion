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

/**
 * Implementation of a gloox IQ handler for the Tyrion service IQ stanza.
 */
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
    virtual gloox::StanzaExtension* clone() const {
      return new XmppService(*this);
    }

    /**
     * Is this a Request or response element?
     */
    inline Format format() { return format_; }
    inline void set_format(Format format) { format_ = format; }

    /**
     * Service type element.
     */
    inline std::string type() const { return type_; }
    inline void set_type(std::string type) { type_ = type; }

    /**
     * Service input element.
     */
    inline std::string input() const { return input_; }
    inline void set_input(std::string input) {
      input_ = input;
      set_format(Request);
    }

    /**
     * Service timeout attribute.
     */
    inline int timeout() const { return timeout_; }
    inline void set_timeout(int timeout) {
      timeout_ = timeout;
      set_format(Request);
    }

    inline std::string user() const { return user_; }
    inline void set_user(std::string user) { user_ = user; }

    /**
     * Service group attribute.
     */
    inline std::string group() const { return group_; }
    inline void set_group(std::string group) { group_ = group; }

    /**
     * Service code attribute.
     */
    inline int code() const { return code_; }
    inline void set_code(int code) {
      code_ = code;
      set_format(Response);
    }

    /**
     * Service output element.
     */
    inline std::string output() const { return output_; }
    inline void set_output(std::string output) {
      output_ = output;
      set_format(Response);
    }

    /**
     * Service error element.
     */
    inline std::string error() const { return error_; }
    inline void set_error(std::string error) {
      error_ = error;
      set_format(Response);
    }

  private:
    Format format_;
    std::string input_;
    std::string user_;
    std::string group_;
    int timeout_;
    int code_;
    std::string output_;
    std::string error_;
    std::string type_;
};

}  // namespace tyrion

#endif  // TYRION_XMPP_SERVICE_H_
