/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_NODE_ENVELOPE_H_
#define _TYRION_NODE_ENVELOPE_H_

#include "envelope.h"
#include "node_process.h"

namespace tyrion {

class NodeEnvelope : public Envelope {
  public:
    NodeEnvelope() : Envelope() {}
    ~NodeEnvelope() {}

    bool Update(const txmpp::XmlElement *stanza);

    bool Check();
    std::string Path();

    NodeProcess* process() { return process_; }
    void set_process(NodeProcess* process) { process_ = process; }

    const txmpp::XmlElement* Response();

  private:
    NodeProcess* process_;
};

}  // tyrion

#endif  // _TYRION_NODE_ENVELOPE_H_
