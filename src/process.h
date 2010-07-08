/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#ifndef _TYRION_PROCESS_H_
#define _TYRION_PROCESS_H_

#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <iostream>

namespace tyrion {

class Process {
  public:
    enum ProcessType {
      Stdout,
      Stderr
    };

    Process(std::string command, bool system = false, int timeout = 30);
    ~Process();

    void Run();
    std::string Read(ProcessType type = Stdout);
    std::string ReadAll(ProcessType type = Stdout);
    void Write(std::string text);

    bool Empty(ProcessType type = Stdout);
    bool TimedOut();
    void Eof();
    int Close();

    void set_timeout(int timeout) { timeout_ = timeout; }
    bool set_user(std::string user, bool set_group = true);
    bool set_group(std::string group);

  private:
    // pipes
    int infd[2];
    int outfd[2][2];
    bool outfdeof[2];
    int statefd[2];
    // pid
    pid_t pid_;
    // options
    uid_t uid_;
    gid_t gid_;
    std::string command_;
    bool system_;
    // info
    time_t start_time;
    int code_;
    bool stdouteof;
    bool timed_out_;
    int timeout_;
};

}  // namespace tyrion

#endif  // _TYRION_PROCESS_H_
