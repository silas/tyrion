/*
 * Copyright (c) 2010, Silas Sewell
 * All rights reserved.
 *
 * This file is subject to the MIT License (see the LICENSE file).
 */

#include "settings.h"

namespace tyrion {

BaseSettings::BaseSettings(const std::string& path) : config_(NULL), path_(path) {
  config_ = new Config(path_);
}

BaseSettings::~BaseSettings() {
  delete config_;
}

bool BaseSettings::HasError() {
  return config_->ParseError() != 0;
}

bool BaseSettings::Validate() {
  return true;
}

bool BaseSettings::Has(const std::string& section, const std::string& name) {
  return config_->Has(section, name);
}

std::string BaseSettings::Get(const std::string& section, const std::string& name,
                              const std::string& default_) {
  return config_->Get(section, name, default_);
}

bool BaseSettings::GetBool(const std::string& section, const std::string& name,
                           bool default_) {
  return config_->Get(section, name, default_ ? "true" : "false") == "true";
}

long BaseSettings::GetInt(const std::string& section, const std::string& name,
                          long default_) {
  return config_->GetInt(section, name, default_);
}

bool BaseSettings::HasRequired(const std::string& section, const std::string& option) {
  if (!Has(section, option) || Get(section, option).empty()) {
    TLOG(ERROR) << "The '" << option << "' option in the '" << section
                << "' section is required.";
    return false;
  }
  return true;
}

Acls::Acls(const std::string& path) : BaseSettings(path) {
}

Settings::Settings(const std::string& path) : BaseSettings(path) {
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
