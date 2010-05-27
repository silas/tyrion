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

namespace tyrion {

XmppServiceManager::XmppServiceManager(gloox::ClientBase* parent) :
    parent_(parent) {
  if (parent_) {
    parent_->registerIqHandler(this, ExtXmppService);
    parent_->disco()->addFeature(XMLNS_IQ_SERVICE);
    parent_->registerStanzaExtension(new XmppService());
  }
}

XmppServiceManager::~XmppServiceManager() {
  if (parent_) {
    parent_->disco()->removeFeature(XMLNS_IQ_SERVICE);
    parent_->removeIqHandler(this, ExtXmppService);
    parent_->removeIDHandler(this);
  }
}

} // namespace tyrion
