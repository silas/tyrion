/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_NODE_ENVELOPE_H_
#define _TYRION_NODE_ENVELOPE_H_

#include "basic.h"
#include "envelope.h"

namespace txmpp {

class XmlElement;

}  // namespace txmpp

namespace tyrion {

class NodeLoop;

class NodeEnvelope : public Envelope {
  public:
    NodeEnvelope(NodeLoop* loop);
    ~NodeEnvelope() {}

    bool Update(const txmpp::XmlElement *stanza);

    bool Check();
    std::string Path();

    const txmpp::XmlElement* Response();
  private:
    NodeLoop* loop_;
    DISALLOW_EVIL_CONSTRUCTORS(NodeEnvelope);
};

}  // namespace tyrion

#endif  // _TYRION_NODE_ENVELOPE_H_
