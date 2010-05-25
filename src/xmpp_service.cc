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

  XmppService::XmppService() : gloox::StanzaExtension(ExtXmppService) {}

  XmppService::XmppService(const gloox::Tag* tag)
    : gloox::StanzaExtension(ExtXmppService)
  {
    if (!tag || tag->name() != "service" || tag->xmlns() != XMLNS_IQ_SERVICE)
      return;

    if (tag->hasAttribute("type"))
      type_ = tag->findAttribute("type");
    else
      return;

    if (tag->hasAttribute("user"))
      user_ = tag->findAttribute("user");

    if (tag->hasAttribute("group"))
      group_ = tag->findAttribute("group");

    if (tag->hasAttribute("timeout"))
    {
      std::string str_timeout = tag->findAttribute("timeout");
      std::istringstream timeout_stream(str_timeout);
      timeout_stream >> timeout_;
    }

    if (tag->hasChild("output") || tag->hasChild("error"))
    {
      SetFormat(Response);

      if (tag->hasChild("output"))
        output_ = tag->findChild("output")->cdata();

      if (tag->hasChild("error"))
        error_ = tag->findChild("error")->cdata();
    }
    else
    {
      SetFormat(Request);

      if (tag->hasChild("input"))
        input_ = tag->findChild("input")->cdata();
    }

    m_valid = true;
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
