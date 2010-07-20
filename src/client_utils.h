/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_CLIENT_UTILS_H_
#define _TYRION_CLIENT_UTILS_H_

#include <string>

namespace tyrion {

class ClientLoop;
class ClientRequest;

/**
 * Exit with status code and cleanup globals.
 */
void ClientExit(int code);

/*
 * Parse CLI options and setup ClientLoop and ClientRequest.
 */
void ClientSetup(int argc, char* argv[], ClientLoop* loop,
                 ClientRequest* request);

}  // namespace tyrion

#endif  // _TYRION_CLIENT_UTILS_H_
