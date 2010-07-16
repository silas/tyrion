/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_SETTINGS_H_
#define _TYRION_SETTINGS_H_

#include <string>
#include "basic.h"
#include "config.h"

namespace tyrion {

class Settings {
  public:
    Settings(const std::string& path);
    virtual ~Settings();

    bool HasError();
    virtual bool Validate();

    bool Has(const std::string& section, const std::string& name);
    std::string Get(const std::string& section, const std::string& name,
                    const std::string& default_ = "");
    bool GetBool(const std::string& section, const std::string& name,
                 bool default_ = false);
    long GetInt(const std::string& section, const std::string& name,
                long default_ = 0);
    bool HasRequired(const std::string& section, const std::string& option);

    inline std::string path() { return path_; }

  protected:
    Config *config_;
    std::string path_;
    DISALLOW_EVIL_CONSTRUCTORS(Settings);
};

}  // namespace tyrion

#endif  // _TYRION_SETTINGS_H_
