/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef TYRION_LOGGING_H_
#define TYRION_LOGGING_H_

#include <sys/stat.h>
#include <time.h>
#include <iostream>
#include <sstream>
#include <string>

#define LOG(level) LogItem(level)

namespace tyrion {

enum LogLevel {
  DEBUG,
  INFO,
  WARNING,
  ERROR,
  CRITICAL
};

class Logging {
  public:
    ~Logging();

    // We use a singleton for the logging class, this returns a logging
    // instance.
    static Logging* Instance();

    void Level(LogLevel level);
    void Level(std::string level, LogLevel default_level = WARNING);

    void Log(LogLevel level, std::string message);

    bool File(std::string path);
    void Stderr(bool enable);

    std::string ToString(LogLevel level);
    LogLevel ToLogLevel(std::string level, LogLevel default_level = WARNING);

  private:
    Logging();
    Logging(const Logging&) {}
    Logging& operator= (const Logging&) {}
    FILE* file_;
    ino_t inode_;
    static Logging* instance_;
    LogLevel level_;
    std::string path_;
    bool stderr_;
};

class LogItem {
  public:
    LogItem(LogLevel level) {
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
    LogLevel level_;
};

}  // namespace tyrion

#endif  // TYRION_LOGGING_H_
