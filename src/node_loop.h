/*
 * Tyrion
 * Copyright 2010 The Tyrion Authors. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The names of the authors may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

  protected:
    NodeLoop();

    void DoReload();
    void DoRequest(ServiceData* service);
    static void *DoRequestInThread(void *arg);
    void DoResponse(ServiceData* service);

    static NodeLoop* instance_;
    int track_;
};

}  // namespace tyrion

#endif  // _TYRION_NODE_LOOP_H_
