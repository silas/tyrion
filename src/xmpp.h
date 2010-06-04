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

namespace tyrion {

/**
 * A wrapper class around the XMPP client the handle various issues and state.
 */
class Xmpp : public gloox::ConnectionListener, gloox::LogHandler {
  public:
    enum State {
      None,
      Connected,
      Disconnected,
      Shutdown
    };

    Xmpp();
    virtual ~Xmpp();

    /**
     * Set base XMPP client settings.
     */
    void Init();

    /**
     * Connect to XMPP server and handle events.
     */
    void Connect();

    /**
     * Disconnect XMPP client in such as way that we can reconnect.
     */
    void Restart();

    /**
     * Disconnect from XMPP client in a permanent way.
     */
    void Stop();

    virtual void SetupHandlers() {}
    virtual void DestroyHandlers() {}

    virtual void onConnect();
    virtual void onDisconnect(gloox::ConnectionError e);
    virtual bool onTLSConnect(const gloox::CertInfo& info);

    virtual void handleLog(gloox::LogLevel level,
                           gloox::LogArea area, const std::string& message);

    /**
     * Return XMPP client.
     */
    gloox::Client *client() { return client_; }

    /**
     * Return current XMPP state.
     */
    State state() { return state_; }

  protected:
    gloox::Client *client_;
    State state_;
};

}  // namespace tyrion

#endif  // TYRION_XMPP_H_
