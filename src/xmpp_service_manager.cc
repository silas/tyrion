/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "xmpp_service_manager.h"

#include "setting.h"
#include "tyrion.h"
#include "utils.h"

namespace tyrion
{

  XmppServiceManager::XmppServiceManager(gloox::ClientBase* parent)
    : m_parent(parent)
  {
    if (m_parent)
    {
      m_parent->registerIqHandler(this, ExtXmppService);
      m_parent->disco()->addFeature(XMLNS_IQ_SERVICE);
      m_parent->registerStanzaExtension(new XmppService());
    }
  }

  XmppServiceManager::~XmppServiceManager()
  {
    if (m_parent)
    {
      m_parent->disco()->removeFeature(XMLNS_IQ_SERVICE);
      m_parent->removeIqHandler(this, ExtXmppService);
      m_parent->removeIDHandler(this);
    }
  }

}
