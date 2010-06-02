/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef TYRION_UTILS_H_
#define TYRION_UTILS_H_

#include <string>

namespace tyrion {
namespace utils {

/**
 * Create a detached thread and return true if it was successfully created.
 */
bool CreateThread(void *func(void*), void *arg);

/**
 * Create a standardized error message which can be return in a service
 * response in stderr.
 */
std::string Error(std::string code, std::string message = "");

/**
 * Return the real path of the specified file.
 */
std::string RealPath(std::string path);

} }  // namespace tyrion::utils

#endif  // TYRION_UTILS_H_
