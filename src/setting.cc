/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "setting.h"

namespace tyrion
{

  void BaseSetting::File(std::string path)
  {
    config_ = new Config(path);
  }

  std::string BaseSetting::Get(std::string section, std::string name, std::string default_)
  {
    return config_->Get(section, name, default_);
  }

  bool BaseSetting::GetBool(std::string section, std::string name, bool default_)
  {
    return config_->Get(section, name, default_ ? "true" : "false") == "true";
  }

  long BaseSetting::GetInt(std::string section, std::string name, long default_)
  {
    return config_->GetInt(section, name, default_);
  }

  bool BaseSetting::HasError()
  {
    return config_->ParseError() < 0;
  }

  Setting* Setting::instance_ = NULL;

  Setting* Setting::Instance()
  {
    if (!instance_)
      instance_ = new Setting;

    return instance_;
  }

}
