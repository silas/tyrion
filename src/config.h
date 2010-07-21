/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_CONFIG_H_
#define _TYRION_CONFIG_H_

#include <map>
#include "basic.h"

namespace tyrion {

class Config {
  public:
    Config(const std::string& filename);

    int ParseError();

    bool Has(const std::string& section, const std::string& name);
    std::string Get(const std::string& section, const std::string& name,
                    const std::string& default_value);
    long GetInt(const std::string& section, const std::string& name,
                long default_value);

  private:
    int error_;
    std::map<std::string, std::string> values_;
    static std::string MakeKey(const std::string& section,
                               const std::string& name);
    static int ValueHandler(void* user, const char* section,
                            const char* name, const char* value);
    DISALLOW_EVIL_CONSTRUCTORS(Config);
};

}  // namespace tyrion

#endif  // _TYRION_CONFIG_H_
