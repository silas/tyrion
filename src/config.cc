/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include "config.h"

#include <cctype>
#include <cstdlib>
#include "third_party/inih/ini.h"

namespace tyrion {

Config::Config(const std::string& filename) : error_(0), values_() {
  error_ = ini_parse(filename.c_str(), ValueHandler, this);
}

int Config::ParseError() {
  return error_;
}

bool Config::Has(const std::string& section, const std::string& name) {
  return values_.count(MakeKey(section, name));
}

std::string Config::Get(const std::string& section, const std::string& name,
                        const std::string& default_value) {
  std::string key = MakeKey(section, name);
  return values_.count(key) ? values_[key] : default_value;
}

long Config::GetInt(const std::string& section, const std::string& name,
                    long default_value) {
  std::string valstr = Get(section, name, "");
  const char* value = valstr.c_str();
  char* end;
  long n = strtol(value, &end, 0);
  return end > value ? n : default_value;
}

std::string Config::MakeKey(const std::string& section,
                            const std::string& name) {
  std::string key = section + "." + name;
  for (size_t i = 0; i < key.length(); i++) {
    key[i] = tolower(key[i]);
  }
  return key;
}

int Config::ValueHandler(void* user, const char* section, const char* name,
                         const char* value) {
  Config* config = (Config*)user;
  config->values_[MakeKey(section, name)] = value;
  return 1;
}

}  // namespace tyrion
