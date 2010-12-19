/*
 * Copyright (c) 2010, Silas Sewell
 * All rights reserved.
 *
 * This file is subject to the MIT License (see the LICENSE file).
 */

#ifndef _TYRION_PROCESS_H_
#define _TYRION_PROCESS_H_

#include "common.h"

namespace tyrion {

class Process {
  public:
    enum Type {
      Stdout,
      Stderr
    };

    Process(std::string command, bool system = false, int timeout = 30);
    ~Process();

    void Run();
    void Write(std::string text, bool eof = true);
    void Update(std::string text, Type type);

    bool Done();
    bool TimedOut();
    int Close();

    void set_timeout(int timeout) { timeout_ = timeout; }
    bool set_user(std::string user, bool set_group = true);
    bool set_group(std::string group);

    // pipes
    int infd[2];
    int outfd[2][2];
    bool outfdeof[2];
    int statefd[2];

  private:
    // pid
    pid_t pid_;
    // options
    uid_t uid_;
    gid_t gid_;
    std::string command_;
    bool system_;
    // info
    time_t start_time_;
    bool timed_out_;
    int timeout_;
    int code_;
    bool ran_;
    bool closed_;
};

}  // namespace tyrion

#endif  // _TYRION_PROCESS_H_
