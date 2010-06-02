/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef TYRION_NODE_VALIDATORS_H_
#define TYRION_NODE_VALIDATORS_H_

#include "setting.h"

namespace tyrion {
namespace node {

class NodeSettingValidator : public SettingValidator {
  public:
    NodeSettingValidator(std::string path);
    void Validate();
};

class NodeAclValidator : public SettingValidator {
  public:
    NodeAclValidator(std::string path);
    void Validate() {}
};

} }  // namespace tyrion::node

#endif  // TYRION_NODE_VALIDATORS_H_
