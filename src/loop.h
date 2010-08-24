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
#include "loop.h"
#include "service_handler.h"
#include "settings.h"
#include "xmpp_pump.h"

namespace tyrion {

class Settings;
class XmppPump;

class Loop : public txmpp::Thread,
             public txmpp::MessageHandler,
             public txmpp::has_slots<> {
  public:
    static const short MSG_LOGIN = 1;
    static const short MSG_OPEN = 2;
    static const short MSG_DISCONNECT = 3;
    static const short MSG_SHUTDOWN = 4;
    static const short MSG_CLOSED = 5;
    static const short MSG_REQUEST = 10;
    static const short MSG_RESPONSE = 11;
    static const short MSG_RESTART = 12;
    static const short MSG_SET_RECONNECT = 13;
    enum State {
      NONE = 0,
      RUNNING,
      RESTARTING,
      STOPPED,
      ERROR
    };
    typedef MessageDataType<Envelope> ServiceData;
    typedef txmpp::TypedMessageData<bool> ReconnectData;

    Loop(pthread_t pthread);
    virtual ~Loop();

    bool Ready();

    void SetReconnect(bool reconnect);
    void Restart();
    void Request(Envelope* envelope);
    void Response(Envelope* envelope);
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

    inline Acls* acls() { return acls_; }
    inline void set_acls(Acls* acls) { acls_ = acls; }

    inline ServiceHandler* service_handler() { return service_handler_; }
    inline void set_service_handler(ServiceHandler* service_handler) {
      service_handler_ = service_handler;
      service_handler_->set_loop(this);
    }

  protected:
    virtual void DoLogin();
    virtual void DoOpen();
    virtual void DoDisconnect();
    virtual void DoShutdown();
    virtual void OnMessage(txmpp::Message* message);
    void DoSetReconnect(ReconnectData* reconnect);
    void DoRestart();
    void DoRequest(ServiceData* service);
    void DoResponse(ServiceData* service);

    void OnSocketClose(int code);

    int track_;
    XmppPump* pump_;
    Acls* acls_;
    ServiceHandler* service_handler_;
    State state_;
    bool reconnect_;
    Settings* settings_;
    pthread_t pthread_;
    int retry_;
    DISALLOW_EVIL_CONSTRUCTORS(Loop);
};


}  // namespace tyrion

#endif  // _TYRION_LOOP_H_
