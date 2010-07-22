/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include "client_settings.h"

#include <txmpp/jid.h>
#include "common.h"

namespace tyrion {

ClientSettings::ClientSettings(const std::string& path) : Settings(path) {
}

bool ClientSettings::Validate() {
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
  if (!jid.resource().empty()) {
    TLOG(ERROR) << "The resource portion of the 'jid' in the 'xmpp' "
                << "section must not exist (node@domain/resource).";
    return false;
  }

  return true;
}

}  // namespace tyrion
