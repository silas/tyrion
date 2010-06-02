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
#include "level.h"

#define LOG(level) tyrion::LogItem(tyrion::level)

namespace tyrion {

class Logging {
  public:
    ~Logging();

    // We use a singleton for the logging class, this returns a logging
    // instance.
    static Logging* Instance();

    void SetLevel(Level level);
    void SetLevel(std::string level, Level default_level = WARNING);

    void Log(Level level, std::string message);

    bool SetFile(std::string path);
    void SetStderr(bool enable);

  private:
    Logging();
    Logging(const Logging&) {}
    Logging& operator= (const Logging&) {}
    FILE *file_;
    ino_t inode_;
    static Logging* instance_;
    Level level_;
    std::string path_;
    bool stderr_;
};

class LogItem {
  public:
    LogItem(Level level) {
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
    Level level_;
};

}  // namespace tyrion

#endif  // TYRION_LOGGING_H_
