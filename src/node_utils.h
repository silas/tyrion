/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_NODE_UTILS_H_
#define _TYRION_NODE_UTILS_H_

#include <string>

namespace tyrion {

class NodeLoop;

void NodeExit(int code);
bool NodeReload();
bool NodeReloadLogging();
NodeLoop* NodeSetup(int argc, char* argv[]);

}  // namespace tyrion

#endif  // _TYRION_NODE_UTILS_H_
