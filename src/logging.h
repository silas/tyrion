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

#ifndef TYRION_LOGGING_H_
#define TYRION_LOGGING_H_

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

#endif  // TYRION_LOGGING_H_
