/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_CLIENT_ENVELOPE_H_
#define _TYRION_CLIENT_ENVELOPE_H_

#include <txmpp/xmlelement.h>
#include "common.h"
#include "envelope.h"

namespace tyrion {

class ClientEnvelope : public Envelope {
  public:
    ClientEnvelope() : Envelope() {}
    ~ClientEnvelope() {}

    bool Update(const txmpp::XmlElement *stanza);

    txmpp::XmlElement* Request();
  private:
    DISALLOW_EVIL_CONSTRUCTORS(ClientEnvelope);
};

}  // tyrion

#endif  // _TYRION_CLIENT_ENVELOPE_H_
