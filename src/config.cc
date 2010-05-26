/*
  Copyright (c) 2009 by Brush Technology
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "config.h"

#include <cctype>
#include <cstdlib>
#include "third_party/inih/ini.h"

namespace tyrion
{

  Config::Config(std::string filename)
  {
    error_ = ini_parse(filename.c_str(), ValueHandler, this);
  }

  int Config::ParseError()
  {
    return error_;
  }

  std::string Config::Get(std::string section, std::string name, std::string default_value)
  {
    std::string key = MakeKey(section, name);
    return values_.count(key) ? values_[key] : default_value;
  }

  long Config::GetInt(std::string section, std::string name, long default_value)
  {
    std::string valstr = Get(section, name, "");
    const char* value = valstr.c_str();
    char* end;
    long n = strtol(value, &end, 0);
    return end > value ? n : default_value;
  }

  std::string Config::MakeKey(std::string section, std::string name)
  {
    std::string key = section + "." + name;
    for (int i = 0; i < key.length(); i++)
      key[i] = tolower(key[i]);
    return key;
  }

  int Config::ValueHandler(void* user, const char* section, const char* name, const char* value)
  {
    Config* config = (Config*)user;
    config->values_[MakeKey(section, name)] = value;
    return 1;
  }

}
