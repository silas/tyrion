/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_NODE_ENVELOPE_H_
#define _TYRION_NODE_ENVELOPE_H_

#include "envelope.h"

namespace tyrion {

class NodeEnvelope : public Envelope {
  public:
    NodeEnvelope() : Envelope() {}
    ~NodeEnvelope() {}

    bool Update(const txmpp::XmlElement *stanza);

    bool Check();
    std::string Path();

    const txmpp::XmlElement* Response();
};

}  // tyrion

#endif  // _TYRION_NODE_ENVELOPE_H_
