/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef TYRION_SETTING_H_
#define TYRION_SETTING_H_

#include <iostream>
#include <string>
#include <vector>
#include "config.h"
#include "level.h"

namespace tyrion {

class BaseSetting {
  public:
    ~BaseSetting();

    void File(std::string path);
    virtual void Reload() {}

    bool HasError();

    bool Has(std::string section, std::string name);
    std::string Get(std::string section, std::string name,
                    std::string default_ = "");
    bool GetBool(std::string section, std::string name, bool default_ = false);
    long GetInt(std::string section, std::string name, long default_ = 0);

    std::string path();
    void set_path(std::string path);

  protected:
    BaseSetting() { path_ = ""; };
    BaseSetting(BaseSetting const&) {};
    BaseSetting& operator=(BaseSetting const&) {};
    Config *config_;
    std::string path_;
};

class Setting : public BaseSetting {
  public:
    static Setting* Instance();
    void Reload();

  private:
    static Setting* instance_;
};

class SettingValidatorIssue {
  public:
    SettingValidatorIssue(std::string text, Level level);
    std::string text() { return text_; }
    Level level() { return level_; }

  private:
    std::string text_;
    Level level_;
};

typedef std::vector<SettingValidatorIssue> SettingValidatorIssueList;

class SettingValidator : public BaseSetting {
  public:
    SettingValidator(std::string path);
    virtual void Validate() {}

    bool IsFatal(Level level = ERROR) { return highest_level_ >= level; }
    void ReportIssues();

    void NewIssue(std::string text, Level level);
    void Issue(std::string section, std::string option, std::string text,
               Level level = ERROR);
    bool Require(std::string section, std::string option);

  protected:
    SettingValidatorIssueList issues_;
    Level highest_level_;
};

}  // namespace tyrion

#endif  // TYRION_SETTING_H_
