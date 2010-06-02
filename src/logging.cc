/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "logging.h"

#include <cstdio>
#include <algorithm>

namespace tyrion {

Logging* Logging::instance_ = NULL;

Logging* Logging::Instance() {
  if (instance_ == 0)
    instance_ = new Logging;

  return instance_;
}

Logging::Logging() {
  file_ = NULL;
  inode_ = NULL;
  level_ = WARNING;
  stderr_ = false;
}

Logging::~Logging() {
  if (file_) fclose(file_);
}

void Logging::SetLevel(Level level) {
  if ((level >= DEBUG) && (level <= CRITICAL)) {
    level_ = level;
  } else {
    level_ = WARNING;
  }
}

void Logging::SetLevel(std::string level, Level default_level) {
  SetLevel(StringToLevel(level, default_level));
}

void Logging::Log(Level level, std::string text) {
  if (level >= level_) {
    struct stat file_stat;

    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    std::string message = asctime(timeinfo);
    message.erase(message.end()-1, message.end());
    message += ": " + LevelToString(level);
    message += ": " + text + "\n";

    if (file_) {
      if (stat(path_.c_str(), &file_stat) != 0 || file_stat.st_ino != inode_) {
        file_ = freopen(path_.c_str(), "a+", file_);
        inode_ = file_stat.st_ino;
      }

      fprintf(file_, "%s", message.c_str());
      fflush(file_);
    }

    if (stderr_)
      std::cerr << message;
  }
}

bool Logging::SetFile(std::string path) {
  path_ = path;
  file_ = fopen(path_.c_str(), "a+");
  return file_ != NULL;
}

void Logging::SetStderr(bool enable) {
  stderr_ = enable;
}

} // namespace tyrion
