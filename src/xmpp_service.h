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

namespace tyrion
{

  const int ExtXmppService = gloox::ExtUser+1;

  enum XmppServiceFormat
  {
    Response,
    Request
  };

  class XmppService : public gloox::StanzaExtension
  {
    public:
      XmppService();
      XmppService(const gloox::Tag* tag);
      virtual ~XmppService() {}
      virtual gloox::StanzaExtension* newInstance(const gloox::Tag* tag) const { return new XmppService(tag); }
      gloox::Tag* tag() const;
      virtual const std::string& filterString() const;
      virtual gloox::StanzaExtension* clone() const { return new XmppService(*this); }
      void SetCode(int code) { code_ = code; SetFormat(Response); }
      void SetError(std::string error) { error_ = error; SetFormat(Response); }
      void SetInput(std::string input) { input_ = input; SetFormat(Request); }
      void SetOutput(std::string output) { output_ = output; SetFormat(Response); }
      void SetTimeout(int timeout) { timeout_ = timeout; SetFormat(Request); }
      void SetType(std::string type) { type_ = type; }
      void SetUser(std::string user) { user_ = user; }
      void SetGroup(std::string group) { group_ = group; }
      void SetFormat(XmppServiceFormat format) { format_ = format; }
      int GetCode() const { return code_; }
      int GetTimeout() const { return timeout_; }
      std::string GetError() const { return error_; }
      std::string GetInput() const { return input_; }
      std::string GetOutput() const { return output_; }
      std::string GetType() const { return type_; }
      std::string GetUser() const { return user_; }
      std::string GetGroup() const { return group_; }

    private:
      bool m_valid;
      std::string type_;
      std::string input_;
      std::string output_;
      std::string error_;
      std::string user_;
      std::string group_;
      int code_;
      int timeout_;
      XmppServiceFormat format_;
  };

}

#endif
