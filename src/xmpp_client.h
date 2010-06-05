/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef TYRION_XMPP_CLIENT_H_
#define TYRION_XMPP_CLIENT_H_

#include <gloox/client.h>

namespace tyrion {

/**
 * Light wrapper around gloox::Client so we can disconnect in a way that
 * allows use to reconnect (needed access to protected methods).
 */
class XmppClient : public gloox::Client {
  public:
    XmppClient(const gloox::JID &jid, const std::string &password) :
        gloox::Client(jid, password) {}

    void Restart() {
      disconnect();
      gloox::ClientBase::disconnect(gloox::ConnNoError);
    }
};

}  // namespace tyrion

#endif  // TYRION_XMPP_CLIENT_H_
