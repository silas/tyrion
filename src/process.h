/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef TYRION_PROCESS_H_
#define TYRION_PROCESS_H_

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

    /**
     * Construct a new process.
     */
    Process(std::string command, bool system = false, int timeout = 30);
    ~Process();

    /**
     * Fork and run process.
     */
    void Run();

    /**
     * Read a buffered text from process stdout/stderr.
     */
    std::string Read(ProcessType type = Stdout);

    /**
     * Read all output from process until EOF or process times out.
     */
    std::string ReadAll(ProcessType type = Stdout);

    /**
     * Write data to process stdin.
     */
    void Write(std::string text);

    /**
     * Check if pipe received EOF.
     */
    bool Empty(ProcessType type = Stdout);

    /**
     * Return true if process was timeout setting was reached.
     */
    bool TimedOut();

    /**
     * Send EOF to process (close pipe).
     */
    void Eof();

    /**
     * Ensure process has close (kill if necessary) and collect information
     * about how the process ended.
     */
    int Close();

    /**
     * Maximum time the process is allowed to run.
     */
    void set_timeout(int timeout) { timeout_ = timeout; }

    /**
     * Lookup UID and set process to use it.
     */
    bool set_user(std::string user, bool set_group = true);

    /**
     * Lookup GID and set process to use it.
     */
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

#endif  // TYRION_PROCESS_H_
