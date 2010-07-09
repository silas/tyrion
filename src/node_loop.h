/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_NODE_LOOP_H_
#define _TYRION_NODE_LOOP_H_

#include "loop.h"
#include "node_envelope.h"
#include "node_service_handler.h"
#include "node_settings.h"
#include "node_xmpp_pump.h"

namespace tyrion {

typedef Loop<NodeEnvelope, NodeSettings, NodeXmppPump> BaseLoop;

class NodeLoop : public BaseLoop {
  public:
    static NodeLoop* Instance();

    inline NodeServiceHandler* service_handler() { return service_handler_; }
    inline void set_service_handler(NodeServiceHandler* service_handler) {
      service_handler_ = service_handler;
    };

  protected:
    NodeLoop();

    void DoReload();
    void DoRequest(ServiceData* service);
    void DoResponse(ServiceData* service);

    static NodeLoop* instance_;
    int track_;
    NodeServiceHandler* service_handler_;
};

}  // namespace tyrion

#endif  // _TYRION_NODE_LOOP_H_
