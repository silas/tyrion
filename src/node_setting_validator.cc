/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "node_setting_validator.h"

namespace tyrion {
namespace node {

NodeSettingValidator::NodeSettingValidator(std::string path) :
  SettingValidator(path) {}

void NodeSettingValidator::Validate() {

  Require("general", "acl");
  Require("general", "service");
  Require("general", "log");

  if (Require("xmpp", "jid") && Get("xmpp", "jid").find("/") == std::string::npos) {
    Issue("xmpp", "jid", "requires a resource (ex: user@host/resource)");
  }
  Require("xmpp", "password");

}

NodeAclValidator::NodeAclValidator(std::string path) :
  SettingValidator(path) {}

} }  // namespace tyrion::node
