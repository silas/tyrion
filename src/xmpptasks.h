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

#ifndef _TYRION_XMPPTASKS_H_
#define _TYRION_XMPPTASKS_H_

#include <txmpp/taskparent.h>
#include <txmpp/xmpptask.h>

namespace tyrion {

class XmppPresenceTask : public txmpp::XmppTask {
  public:
    explicit XmppPresenceTask(txmpp::TaskParent *parent);
    virtual ~XmppPresenceTask();
    virtual int ProcessStart();
    virtual int ProcessResponse();
    bool HandleStanza(const txmpp::XmlElement *stanza);
};

class XmppServiceTask : public txmpp::XmppTask {
  public:
    explicit XmppServiceTask(txmpp::TaskParent *parent);
    virtual ~XmppServiceTask();
    virtual int ProcessStart();
    virtual int ProcessResponse();
    bool HandleStanza(const txmpp::XmlElement *stanza);
    bool IsValid(const txmpp::XmlElement *stanza);
};

}  // namespace tyrion

#endif  // _TYRION_XMPPTASK_H_
