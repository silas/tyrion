/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "setting_validator.h"

#include "logging.h"

namespace tyrion {

SettingValidatorIssue::SettingValidatorIssue(std::string text, Level level) {
  text_ = text;
  level_ = level;
}

SettingValidator::SettingValidator(std::string path) {
  highest_level_ = DEBUG;
  OpenFile(path);
  Validate();
}

void SettingValidator::NewIssue(std::string text, Level level) {
  if (level > highest_level_) highest_level_ = level;
  issues_.push_back(SettingValidatorIssue(text, level));
}

void SettingValidator::ReportIssues() {
  for (int i = 0; i < issues_.size(); i++) {
    LogItem(issues_[i].level()) << issues_[i].text();
  }
}

void SettingValidator::Issue(std::string section, std::string option, std::string text,
                           Level level) {
  NewIssue("The " + option + " option in the " + section + " section " + text +
           ".", level);
}

bool SettingValidator::Require(std::string section, std::string option) {
  if (Get(section, option).empty()) {
    Issue(section, option, "is required");
    return false;
  }
  return true;
}

} // namespace tyrion
