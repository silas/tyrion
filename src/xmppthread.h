/*
 * Tyrion
 * Copyright 2010, Silas Sewell
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
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

#ifndef _TYRION_XMPPTHREAD_H_
#define _TYRION_XMPPTHREAD_H_

#include <iostream>
#include <txmpp/thread.h>
#include <txmpp/xmppclientsettings.h>
#include "xmpppump.h"

namespace tyrion {

class XmppThread : public txmpp::Thread, XmppPumpNotify, txmpp::MessageHandler,
                   public txmpp::has_slots<> {
  public:
    enum State {
      NONE,
      STARTED,
      STOPPED,
      STOPPED_ERROR,
      SHUTDOWN,
      SHUTDOWN_ERROR
    };

    XmppThread();
    ~XmppThread();

    txmpp::XmppClient* client() { return pump_->client(); }

    void ProcessMessages(int cms);
    void Login(const txmpp::XmppClientSettings & xcs, int delay = 0);
    void Disconnect();

    void SocketClose(int code);
    void Raise(State state);

    State state() { return state_; }

  private:
    XmppPump* pump_;
    State state_;

    void OnStateChange(txmpp::XmppEngine::State state, int code = 0);
    void OnMessage(txmpp::Message* pmsg);
};

}  // namespace tyrion

#endif  // _TYRION_XMPPTHREAD_H_
