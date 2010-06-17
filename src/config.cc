/*
 * Tyrion
 * Copyright 2009, Brush Technology
 * Copyright 2010, Silas Sewell
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include <cctype>
#include <cstdlib>
#include "third_party/inih/ini.h"

namespace tyrion {

Config::Config(std::string filename) {
  error_ = ini_parse(filename.c_str(), ValueHandler, this);
}

int Config::ParseError() {
  return error_;
}

bool Config::Has(std::string section, std::string name) {
  return values_.count(MakeKey(section, name));
}

std::string Config::Get(std::string section, std::string name,
                        std::string default_value) {
  std::string key = MakeKey(section, name);
  return values_.count(key) ? values_[key] : default_value;
}

long Config::GetInt(std::string section, std::string name,
                    long default_value) {
  std::string valstr = Get(section, name, "");
  const char* value = valstr.c_str();
  char* end;
  long n = strtol(value, &end, 0);
  return end > value ? n : default_value;
}

std::string Config::MakeKey(std::string section, std::string name) {
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

} // namespace tyrion
