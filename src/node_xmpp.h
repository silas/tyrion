/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef TYRION_NODE_XMPP_H_
#define TYRION_NODE_XMPP_H_

#include "node_xmpp_service_manager.h"
#include "xmpp.h"

namespace tyrion {
namespace node {

class Xmpp : public tyrion::Xmpp {
  public:
    void SetupHandlers();
    void DestroyHandlers();

  private:
    node::XmppServiceManager *service_manager_;
};

} }  // namespace tyrion::node

#endif  // TYRION_NODE_XMPP_H_
