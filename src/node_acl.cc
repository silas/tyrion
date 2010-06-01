/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "node_acl.h"

#include "logging.h"
#include "tyrion.h"

namespace tyrion {
namespace node {

Acl* Acl::instance_ = NULL;

Acl* Acl::Instance() {
  if (!instance_)
    instance_ = new Acl;
  return instance_;
}

void Acl::Reload() {
  if (instance_) {
    LOG(INFO) << "Reloading ACLs...";
    Acl *old_instance = instance_;
    Acl *new_instance = new Acl;
    std::string path = Setting::Instance()->Get("general", "acl", ACL_PATH);
    new_instance->File(path);
    if (new_instance->HasError()) {
      LOG(WARNING) << "Unable to reload ACLs...";
      delete(new_instance);
    } else {
      instance_ = new_instance;
      delete(old_instance);
    }
  }
}

} } // namespace tyrion::node
