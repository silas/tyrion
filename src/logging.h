/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_LOGGING_H_
#define _TYRION_LOGGING_H_

#include <cstdio>
#include <sys/stat.h>
#include <time.h>
#include <iostream>
#include <sstream>
#include <string>

#define TLOG(level) tyrion::LogItem(tyrion::Logging::level)

namespace tyrion {

class Logging {
  public:
    enum Level {
      DEBUG = 0,
      INFO,
      WARNING,
      ERROR,
      CRITICAL,
      NONE
    };

    ~Logging();

    inline static Logging* New() {
      return new Logging;
    }

    inline static Logging* Instance(Logging* instance = NULL) {
      if (instance != NULL) {
        instance_ = instance;
      } else if (instance_ == NULL) {
        instance_ = New();
      }
      return instance_;
    }

    bool Debug(Level level);
    bool File(const std::string& path, Level level);

    void Log(Level level, const std::string& message);

    inline Level debug_level() { return debug_level_; }
    inline Level file_level() { return file_level_; }

    static std::string LevelToString(Level level);
    static Level StringToLevel(std::string level, Level default_level);

  private:
    Logging();
    Logging(const Logging&) {}
    static Logging* instance_;
    Level debug_level_;
    Level file_level_;
    Level lowest_level_;
    std::string file_path_;
    FILE *file_;
};

class LogItem {
  public:
    LogItem(Logging::Level level) {
      level_ = level;
    }
    ~LogItem() {
      Logging::Instance()->Log(level_, buffer_.str());
    }

    template <typename T>
    LogItem & operator<<(T const & value) {
      buffer_ << value;
      return *this;
    }   

  private:
    std::ostringstream buffer_;
    Logging::Level level_;
};

}  // namespace tyrion

#endif  // _TYRION_LOGGING_H_
