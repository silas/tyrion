/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include "envelope.h"

#include <txmpp/constants.h>
#include "utils.h"

namespace tyrion {

Envelope::Envelope() :
  jid_(txmpp::STR_EMPTY),
  id_(txmpp::STR_EMPTY),
  input_(txmpp::STR_EMPTY),
  output_(txmpp::STR_EMPTY),
  error_(txmpp::STR_EMPTY),
  type_(txmpp::STR_EMPTY),
  user_(txmpp::STR_EMPTY),
  group_(txmpp::STR_EMPTY),
  code_(0),
  timeout_(PROCESS_TIMEOUT),
  retry_(0) {
}

int Envelope::Retry() {
  if (retry_ <= 0) {
    retry_ = 1;
  } else if (retry_ < 32) {
    retry_ *= 2;
  }
  return retry_;
}

}  // namespace tyrion
