/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_NODE_SERVICE_HANDLER_H_
#define _TYRION_NODE_SERVICE_HANDLER_H_

#include <string>
#include "node_envelope.h"

namespace tyrion {

class NodeServiceHandler {
  public:
    NodeServiceHandler(NodeEnvelope* service);
    ~NodeServiceHandler();

    /**
     * Create and run the service process.
     */
    void Run();

  private:
    NodeEnvelope* service_;
};

}  // namespace tyrion

#endif  // _TYRION_NODE_SERVICE_HANDLER_H_
