/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "utils.h"

#include <limits.h>
#include <pthread.h>
#include <stdlib.h>

namespace tyrion {
namespace utils {

std::string Error(std::string code, std::string message) {
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

std::string RealPath(std::string path) {
  char buffer[PATH_MAX + 1];
  char *rc = realpath(path.c_str(), buffer);

  if (rc)
    return std::string(buffer);
  return "";
}

} } // namespace tyrion::utils
