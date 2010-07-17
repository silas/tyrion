/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_BASIC_H_
#define _TYRION_BASIC_H_

#include <txmpp/constructormagic.h>
#include <txmpp/messagequeue.h>
#include "constants.h"

namespace tyrion {

template <class T>
class MessageDataType : public txmpp::MessageData {
 public:
  explicit MessageDataType(T* data) : data_(data) {}
  const T* data() const { return data_; }
  T* data() { return data_; }
 private:
  T* data_;
  DISALLOW_EVIL_CONSTRUCTORS(MessageDataType);
};

}  // namespace tyrion

#endif  // _TYRION_BASIC_H_
