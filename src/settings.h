/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_SETTINGS_H_
#define _TYRION_SETTINGS_H_

#include <iostream>
#include <string>
#include "config.h"
#include "logging.h"

namespace tyrion {

template <class T>
class Settings {
  public:
    ~Settings() {
      if (config_) {
        delete config_;
        config_ = NULL;
      }
    }

    static T* New() {
      return new T;
    }

    static T* Instance(T* instance = NULL) {
      static T* instance_;
      if (instance != NULL) {
        instance_ = instance;
      } else if (!instance_) {
        instance_ = New();
      }
      return instance_;
    }

    bool Setup(const std::string& path) {
      path_ = path;
      config_ = new Config(path_);
      return config_->ParseError() >= 0;
    }

    bool Validate() {
      T* instance_ = Settings<T>::Instance();
      return ValidateInstance(instance_);
    }

    virtual bool ValidateInstance(T* instance) {
      return true;
    }

    bool Has(const std::string& section, const std::string& name) {
      return config_->Has(section, name);
    }

    std::string Get(const std::string& section, const std::string& name,
                    const std::string& default_ = "") {
      return config_->Get(section, name, default_);
    }

    bool GetBool(const std::string& section, const std::string& name,
                 bool default_ = false) {
      return config_->Get(section, name, default_ ?
                          "true" :"false") == "true";
    }

    long GetInt(const std::string& section, const std::string& name,
                long default_ = 0) {
      return config_->GetInt(section, name, default_);
    }

    bool HasRequired(const std::string& section, const std::string& option) {
      if (!Has(section, option) || Get(section, option).empty()) {
        TLOG(ERROR) << "The '" << option << "' option in the '" << section
                    << "' section is required.";
        return false;
      }
      return true;
    }

    inline std::string path() { return path_; }

  protected:
    Settings() { config_ = NULL; }
    Config *config_;
    std::string path_;
};

}  // namespace tyrion

#endif  // _TYRION_SETTINGS_H_
