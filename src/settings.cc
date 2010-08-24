/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include "settings.h"

namespace tyrion {

Settings::Settings(const std::string& path) : config_(NULL), path_(path) {
  config_ = new Config(path_);
}

Settings::~Settings() {
  delete config_;
}

bool Settings::HasError() {
  return config_->ParseError() != 0;
}

bool Settings::Validate() {
  return true;
}

bool Settings::Has(const std::string& section, const std::string& name) {
  return config_->Has(section, name);
}

std::string Settings::Get(const std::string& section, const std::string& name,
                          const std::string& default_) {
  return config_->Get(section, name, default_);
}

bool Settings::GetBool(const std::string& section, const std::string& name,
                      bool default_) {
  return config_->Get(section, name, default_ ? "true" : "false") == "true";
}

long Settings::GetInt(const std::string& section, const std::string& name,
                     long default_) {
  return config_->GetInt(section, name, default_);
}

bool Settings::HasRequired(const std::string& section, const std::string& option) {
  if (!Has(section, option) || Get(section, option).empty()) {
    TLOG(ERROR) << "The '" << option << "' option in the '" << section
                << "' section is required.";
    return false;
  }
  return true;
}

Acls::Acls(const std::string& path) : Settings(path) {
}

Settings::Settings(const std::string& path) : Settings(path) {
}

bool Settings::Validate() {
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

}  // namespace tyrion
