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

#include "logging.h"

namespace tyrion {

BaseSetting::~BaseSetting() {
  if (config_)
    delete(config_);
}

void BaseSetting::File(std::string path) {
  path_ = path;
  config_ = new Config(path_);
}

bool BaseSetting::HasError() {
  return config_->ParseError() < 0;
}

bool BaseSetting::Has(std::string section, std::string name) {
  return config_->Has(section, name);
}

std::string BaseSetting::Get(std::string section, std::string name,
                             std::string default_) {
  return config_->Get(section, name, default_);
}

bool BaseSetting::GetBool(std::string section, std::string name,
                          bool default_) {
  return config_->Get(section, name, default_ ? "true" : "false") == "true";
}

long BaseSetting::GetInt(std::string section, std::string name,
                         long default_) {
  return config_->GetInt(section, name, default_);
}

std::string BaseSetting::path() {
  return path_;
}

void BaseSetting::set_path(std::string path) {
  path_ = path;
}

Setting* Setting::instance_ = NULL;

Setting* Setting::Instance() {
  if (!instance_)
    instance_ = new Setting;
  return instance_;
}

void Setting::Reload() {
  if (instance_) {
    LOG(INFO) << "Reloading settings...";
    Setting *old_instance = instance_;
    Setting *new_instance = new Setting;
    new_instance->File(old_instance->path());
    if (new_instance->HasError()) {
      LOG(WARNING) << "Unable to reload settings...";
      delete(new_instance);
    } else {
      instance_ = new_instance;
      delete(old_instance);
    }
  }
}

} // namespace tyrion
