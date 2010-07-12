/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include "settings.h"

#include <string>
#include "logging.h"

namespace tyrion {

Settings::Settings(const std::string& path) {
  path_ = path;
  config_ = new Config(path_);
}

Settings::~Settings() {
  delete config_;
}

bool Settings::HasError() {
  return config_->ParseError() >= 0;
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

}  // namespace tyrion
