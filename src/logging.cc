/*
 * Tyrion
 * Copyright 2010, Silas Sewell
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
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

#include "logging.h"

#include <algorithm>

namespace tyrion {

Logging* Logging::instance_ = NULL;

Logging::Logging() {
  file_ = NULL;
  debug_level_ = NONE;
  file_level_ = NONE;
}

Logging::~Logging() {
  if (file_) fclose(file_);
}

void Logging::Log(Level level, const std::string& text) {
  if (level >= lowest_level_) {
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    if (level >= file_level_) {
      std::string message = asctime(timeinfo);
      message.erase(message.end()-1, message.end());
      message += ": " + LevelToString(level);
      message += ": " + text + "\n";

      fprintf(file_, "%s", message.c_str());
      fflush(file_);
    }

    if (level >= debug_level_) {
      std::cerr << text << std::endl;
    }
  }
}

bool Logging::File(const std::string& path, Level level) {
  if (level < DEBUG || level > NONE) return false;
  if (level < lowest_level_) lowest_level_ = level;
  file_level_ = level;
  if (level == NONE && file_) {
    fclose(file_);
  } else {
    file_path_ = path;
    file_ = fopen(file_path_.c_str(), "a+");
    return file_ == NULL;
  }
  return true;
}

bool Logging::Debug(Level level) {
  if (level < DEBUG || level > NONE) return false;
  if (level < lowest_level_) lowest_level_ = level;
  debug_level_ = level;
  return true;
}

std::string Logging::LevelToString(Level level) {
  switch(level) {
    case DEBUG:
      return "Debug";
    case INFO:
      return "Info";
    case WARNING:
      return "Warning";
    case ERROR:
      return "Error";
    case CRITICAL:
      return "Critical";
    default:
      return "None";
  }
}

}  // namespace tyrion
