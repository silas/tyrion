/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "xmpp_service.h"

#include <sstream>

namespace tyrion
{

  XmppService::XmppService() : gloox::StanzaExtension(ExtXmppService)
  {
    type_ = "";
    input_ = "";
    output_ = "";
    error_ = "";
    user_ = "";
    group_ = "";
    code_ = -1;
    timeout_ = 0;
    format_ = None;
  }

  XmppService::XmppService(const gloox::Tag* tag)
    : gloox::StanzaExtension(ExtXmppService)
  {
    type_ = tag->hasAttribute("type") ? tag->findAttribute("type") : "";
    user_ = tag->hasAttribute("user") ? tag->findAttribute("user") : "";
    group_ = tag->hasAttribute("group") ? tag->findAttribute("group") : "";

    code_ = -1;
    if (tag->hasAttribute("code"))
    {
      std::string code_str = tag->findAttribute("code");
      std::istringstream code_stream(code_str);
      code_stream >> code_;
    }

    timeout_ = 0;
    if (tag->hasAttribute("timeout"))
    {
      std::string timeout_str = tag->findAttribute("timeout");
      std::istringstream timeout_stream(timeout_str);
      timeout_stream >> timeout_;
    }

    output_ = tag->hasChild("output") ? tag->findChild("output")->cdata() : "";
    error_ = tag->hasChild("error") ? tag->findChild("error")->cdata() : "";
    input_ = tag->hasChild("input") ? tag->findChild("input")->cdata() : "";

    if (tag->hasChild("output") || tag->hasChild("error"))
      SetFormat(Response);
    else
      SetFormat(Request);
  }

  const std::string& XmppService::filterString() const
  {
    static const std::string filter = "|/iq/service[@xmlns='"
      + XMLNS_IQ_SERVICE + "']";

    return filter;
  }

  gloox::Tag* XmppService::tag() const
  {
    gloox::Tag* t= new gloox::Tag("service", gloox::XMLNS, XMLNS_IQ_SERVICE);
    t->addAttribute("type", type_);

    if (format_ == Request)
    {
      gloox::Tag* i = new gloox::Tag("input");
      i->setCData(input_);
      t->addChild(i);
    }
    else
    {
      gloox::Tag* o = new gloox::Tag("output");
      gloox::Tag* e = new gloox::Tag("error");

      o->setCData(output_);
      e->setCData(error_);

      t->addAttribute("code", code_);
      t->addChild(o);
      t->addChild(e);
    }

    return t;
  }

}
