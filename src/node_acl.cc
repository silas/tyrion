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

void Acl::Reload() {
  if (instance_) {
    LOG(INFO) << "Reloading ACLs...";

    // Setup instances for switch
    Acl *old_instance = instance_;
    Acl *new_instance = new Acl;

    // Try to open new ACL configuration file
    std::string path = Setting::Instance()->Get("general", "acl");
    new_instance->OpenFile(path);

    if (new_instance->HasError()) {
      // Warning and fail if we were unable to load
      LOG(WARNING) << "Unable to reload ACLs...";
      delete(new_instance);
    } else {
      // Cleanup if reload worked
      instance_ = new_instance;
      delete(old_instance);
    }
  }
}

} } // namespace tyrion::node
