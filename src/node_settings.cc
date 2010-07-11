/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include "node_settings.h"

#include <txmpp/jid.h>
#include "constants.h"

bool NodeSettings::Validate() {
  if (!HasRequired(SETTING_GENERAL, SETTING_ACL_PATH)) return false;
  if (!HasRequired(SETTING_GENERAL, SETTING_SERVICE_PATH)) return false;
  if (!HasRequired(SETTING_GENERAL, SETTING_LOG_PATH)) return false;

  if (!HasRequired(SETTING_XMPP, SETTING_JID)) return false;
  if (!HasRequired(SETTING_XMPP, SETTING_PASSWORD)) return false;

  txmpp::Jid jid(Get(SETTING_XMPP, SETTING_JID));
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
