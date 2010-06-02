/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef TYRION_XMPP_H_
#define TYRION_XMPP_H_

#include <gloox/client.h>
#include <gloox/connectionlistener.h>
#include <gloox/disco.h>
#include <gloox/gloox.h>
#include <gloox/lastactivity.h>
#include <gloox/loghandler.h>
#include <gloox/logsink.h>
#include <gloox/connectiontcpclient.h>
#include <gloox/connectionsocks5proxy.h>
#include <gloox/connectionhttpproxy.h>
#include "xmpp_client.h"

namespace tyrion {

class Xmpp : public gloox::ConnectionListener, gloox::LogHandler {
  public:
    enum State {
      None,
      Connected,
      Disconnected,
      Shutdown,
      Reload
    };

    Xmpp();
    virtual ~Xmpp();

    void Init();
    void Connect();
    void Restart();
    void Stop();

    virtual void SetupHandlers() {}
    virtual void DestroyHandlers() {}

    virtual void onConnect();
    virtual void onDisconnect(gloox::ConnectionError e);
    virtual bool onTLSConnect(const gloox::CertInfo& info);

    virtual void handleLog(gloox::LogLevel level,
                           gloox::LogArea area, const std::string& message);

    gloox::Client* client() { return client_; }

    State state() { return state_; }

  protected:
    XmppClient *client_;
    State state_;
};

}  // namespace tyrion

#endif  // TYRION_XMPP_H_
