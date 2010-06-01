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
#include "config.h"

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

}  // namespace tyrion

#endif  // TYRION_SETTING_H_
