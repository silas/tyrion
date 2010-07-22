/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_LOOP_H_
#define _TYRION_LOOP_H_

#include <txmpp/xmppengine.h>
#include "common.h"
#include "xmpp_pump.h"

namespace tyrion {

class Settings;

class Loop : public txmpp::Thread,
             public txmpp::MessageHandler,
             public txmpp::has_slots<> {
  public:
    static const short MSG_LOGIN = 1;
    static const short MSG_OPEN = 2;
    static const short MSG_DISCONNECT = 3;
    static const short MSG_SHUTDOWN = 4;
    static const short MSG_CLOSED = 5;
    enum State {
      NONE = 0,
      RUNNING,
      RESTARTING,
      STOPPED,
      ERROR
    };

    Loop(pthread_t pthread);
    virtual ~Loop();

    bool Ready();

    void Login();
    void Disconnect();

    void ProcessMessages(int cms);
    void OnStateChange(txmpp::XmppEngine::State state, int code = 0);

    inline txmpp::XmppClient* client() { return pump_->client(); }
    inline State state() { return state_; }

    inline Settings* settings() { return settings_; }
    inline void set_settings(Settings* settings) {
      settings_ = settings;
    }

  protected:
    virtual void DoLogin();
    virtual void DoOpen();
    virtual void DoDisconnect();
    virtual void DoShutdown();
    virtual void OnMessage(txmpp::Message* message);

    void OnSocketClose(int code);

    virtual void SetupPump() {}

    XmppPump* pump_;
    State state_;
    Settings* settings_;
    pthread_t pthread_;
    int retry_;
    DISALLOW_EVIL_CONSTRUCTORS(Loop);
};


}  // namespace tyrion

#endif  // _TYRION_LOOP_H_
