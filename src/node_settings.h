/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_NODE_SETTINGS_H_
#define _TYRION_NODE_SETTINGS_H_

#include "settings.h"

namespace tyrion {

class NodeAcls : public Settings {
  public:
    NodeAcls(const std::string& path);
};

class NodeSettings : public Settings {
  public:
    NodeSettings(const std::string& path);
    bool Validate();
};

}  // namespace tyrion

#endif  // _TYRION_NODE_SETTINGS_H_
