/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef TYRION_SETTING_VALIDATOR_H_
#define TYRION_SETTING_VALIDATOR_H_

#include <string>
#include <vector>
#include "level.h"
#include "setting.h"

namespace tyrion {

/**
 * Issue with option which includes both severity level and a text
 * description.
 */
class SettingValidatorIssue {
  public:
    /**
     * Construct an new issue.
     */
    SettingValidatorIssue(std::string text, Level level);

    /**
     * Description of issue.
     */
    inline std::string text() { return text_; }

    /**
     * Severity of issue.
     */
    inline Level level() { return level_; }

  private:
    std::string text_;
    Level level_;
};

typedef std::vector<SettingValidatorIssue> SettingValidatorIssueList;

class SettingValidator : public BaseSetting {
  public:
    /**
     * Construct non-singleton representation of configuration file.
     */
    SettingValidator(std::string path);

    /**
     * Implemented by children to validate a settings file.
     */
    virtual void Validate() {}

    /**
     * Pass the severity level which represents a fatal error and check if any
     * of the issues meet it.
     */
    bool IsFatal(Level level = ERROR) { return highest_level_ >= level; }

    /**
     * Send issues to Logging instance.
     */
    void ReportIssues();

    /**
     * Create a new issue.
     */
    void NewIssue(std::string text, Level level);

    /**
     * Create a new issue with some additional helper logic.
     */
    void Issue(std::string section, std::string option, std::string text,
               Level level = ERROR);

    /**
     * Require a setting or create a new issue.
     */
    bool Require(std::string section, std::string option);

  protected:
    SettingValidatorIssueList issues_;
    Level highest_level_;
};

}  // namespace tyrion

#endif  // TYRION_VALIDATOR_H_
