/*
 * Tyrion
 * Copyright 2010 The Tyrion Authors. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The names of the authors may not be used to endorse or promote products
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

#include "utils.h"

#include <limits.h>
#include <pthread.h>
#include <stdlib.h>

namespace tyrion {

std::string CreateError(std::string code, std::string message) {
  Escape(message);
  return "Error: org.tyrion.error." + code + (!message.empty() ? ": " +
      message : message) + "\n";
}

bool CreateThread(void *func(void*), void *arg) {
  pthread_t handler;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  int rc = pthread_create(&handler, &attr, func, arg);

  pthread_attr_destroy(&attr);

  return rc == 0;
}

void Escape(std::string& text) {
  StringReplace(text, "\n", "\\n");
  StringReplace(text, "\r", "\\r");
}

std::string RealPath(const std::string& path) {
  char buffer[PATH_MAX + 1];
  char *rc = realpath(path.c_str(), buffer);

  return rc ? std::string(buffer) : "";
}

void StringReplace(std::string& text, const std::string& search,
             const std::string& replace) {
  std::string::size_type pos = text.find(search);

  while (pos != std::string::npos) {
    text.replace(pos, search.length(), replace);
    pos = text.find(search, pos+search.length());
  }
}

}  // namespace tyrion
