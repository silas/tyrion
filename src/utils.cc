/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
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
