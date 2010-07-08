/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
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

    if (level >= file_level_ && file_ != NULL) {
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
  if (level < lowest_level_) lowest_level_ = level;

  file_level_ = level;

  if (level == NONE && file_ != NULL) {
    fclose(file_);
    file_ = NULL;
    return true;
  } else if (level != NONE && file_ == NULL) {
    file_path_ = path;
    file_ = fopen(file_path_.c_str(), "a+");
    return file_ != NULL;
  }

  return false;
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

Logging::Level Logging::StringToLevel(std::string level, Level default_level) {
  std::transform(level.begin(), level.end(), level.begin(), ::tolower);

  if (level == "critical") {
    return CRITICAL;
  } else if (level == "error") {
    return ERROR;
  } else if (level == "warning") {
    return WARNING;
  } else if (level == "info") {
    return INFO;
  } else if (level == "debug") {
    return DEBUG;
  } else {
    return default_level;
  }
}

}  // namespace tyrion
