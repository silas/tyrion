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

#ifndef TYRION_CLIENT_SETTINGS_H_
#define TYRION_CLIENT_SETTINGS_H_

#include <txmpp/jid.h>
#include "constants.h"
#include "settings.h"

namespace tyrion {

class ClientSettings : public Settings<ClientSettings> {
  public:
    bool ValidateInstance(ClientSettings* s) {
      if (!s->HasRequired(STR_XMPP, STR_JID)) return false;
      if (!s->HasRequired(STR_XMPP, STR_PASSWORD)) return false;

      txmpp::Jid jid(s->Get(STR_XMPP, STR_JID));
      if (jid.node().empty()) {
        TLOG(ERROR) << "The node portion of the 'jid' in the 'xmpp' section is"
                    << " required (node@domain/resource).";
        return false;
      }
      if (jid.domain().empty()) {
        TLOG(ERROR) << "The domain portion of the 'jid' in the 'xmpp' section "
                    << "is required (node@domain/resource).";
        return false;
      }
      if (!jid.resource().empty()) {
        TLOG(ERROR) << "The resource portion of the 'jid' in the 'xmpp' "
                    << "section must not exist (node@domain/resource).";
        return false;
      }

      return true;
    }

};

}  // namespace tyrion

#endif  // TYRION_CLIENT_SETTINGS_H_
