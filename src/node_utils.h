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
class NodeSettings;

void NodeExit(int code);
bool NodeSetupLogging(NodeSettings* settings, bool reload);
int NodeSetup(int argc, char* argv[], NodeLoop* loop, bool reload);

}  // namespace tyrion

#endif  // _TYRION_NODE_UTILS_H_
