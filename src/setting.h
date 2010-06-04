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
#include "config.h"
#include "level.h"

namespace tyrion {

class BaseSetting {
  public:
    ~BaseSetting();

    /**
     * Open configuration file and parse settings.
     */
    void OpenFile(std::string path);

    /**
     * Check if there were errors parsing settings file.
     */
    bool HasError();

    /**
     * Reload configuration settings.
     */
    virtual void Reload() {}

    /**
     * Check if option exists in configuration file.
     */
    bool Has(std::string section, std::string name);

    /**
     * Return string representation of option.
     */
    std::string Get(std::string section, std::string name,
                    std::string default_ = "");

    /**
     * Return boolean representation of option.
     */
    bool GetBool(std::string section, std::string name, bool default_ = false);

    /**
     * Return int representation of option.
     */
    long GetInt(std::string section, std::string name, long default_ = 0);

    /**
     * Configuration file path.
     */
    std::string path();
    void set_path(std::string path);

  protected:
    BaseSetting() { path_ = ""; };
    BaseSetting(BaseSetting const&) {};
    Config *config_;
    std::string path_;
};

/**
 * General settings for application.
 */
class Setting : public BaseSetting {
  public:
    static Setting* Instance();
    void Reload();

  private:
    static Setting* instance_;
};

}  // namespace tyrion

#endif  // TYRION_SETTING_H_
