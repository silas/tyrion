/*
 * Tyrion
 * Copyright 2010 The Tyrion Authors. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The names of the authors may not be used to endorse or promote products
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

#ifndef TYRION_SETTINGS_H_
#define TYRION_SETTINGS_H_

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

#endif  // TYRION_SETTINGS_H_
