/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_NODE_SETTINGS_H_
#define _TYRION_NODE_SETTINGS_H_

#include <txmpp/jid.h>
#include "constants.h"
#include "settings.h"

namespace tyrion {

class NodeAcls : public Settings<NodeAcls> {};

class NodeSettings : public Settings<NodeSettings> {
  public:
    bool ValidateInstance(NodeSettings* s) {
      if (!s->HasRequired(STR_GENERAL, STR_ACL_PATH)) return false;
      if (!s->HasRequired(STR_GENERAL, STR_SERVICE_PATH)) return false;
      if (!s->HasRequired(STR_GENERAL, STR_LOG_PATH)) return false;

      if (!s->HasRequired(STR_XMPP, STR_JID)) return false;
      if (!s->HasRequired(STR_XMPP, STR_PASSWORD)) return false;

      txmpp::Jid jid(s->Get(STR_XMPP, STR_JID));
      if (jid.node().empty()) {
        TLOG(ERROR) << "The node portion of the 'jid' in the 'xmpp' section is"
                    << " required (node@domain/resource).";
        return false;
      }
      if (jid.domain().empty()) {
        TLOG(ERROR) << "The domain portion of the 'jid' in the 'xmpp' section "
                    << "is required (node@domain/resource).";
        return false;
      }
      if (jid.resource().empty()) {
        TLOG(ERROR) << "The resource portion of the 'jid' in the 'xmpp' "
                    << "section is required (node@domain/resource).";
        return false;
      }

      return true;
    }

};

}  // namespace tyrion

#endif  // _TYRION_NODE_SETTINGS_H_
