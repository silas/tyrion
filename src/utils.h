/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_UTILS_H_
#define _TYRION_UTILS_H_

#include <string>

namespace tyrion {

/**
 * Create a standardized error message which can be return in a service
 * response in stderr.
 */
std::string CreateError(std::string code, std::string message = "");

/**
 * Create a detached thread and return true if it was successfully created.
 */
bool CreateThread(void *func(void*), void *arg);

/**
 * Escape various hidden and whitespace characters.
 */
void Escape(std::string& text);

/**
 * Return the real path of the specified file.
 */
std::string RealPath(const std::string& path);

/**
 * In-place string search and replace.
 */
void StringReplace(std::string& text, const std::string& search,
                   const std::string& replace);

}  // namespace tyrion

#endif  // _TYRION_UTILS_H_
