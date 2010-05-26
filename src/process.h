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

namespace tyrion
{

  class Process
  {
    public:
      enum ProcessType
      {
        Stdout,
        Stderr
      };
      Process(std::string command, bool system = false, int timeout = 30);
      ~Process();
      std::string Read(ProcessType type = Stdout);
      std::string ReadAll(ProcessType type = Stdout);
      void Run();
      bool Empty(ProcessType type = Stdout);
      void Write(std::string text);
      bool TimedOut();
      void Eof();
      int Close();
      bool SetUser(std::string user, bool set_group = true);
      bool SetGroup(std::string group);
      void SetTimeout(int timeout) { timeout_ = timeout; }

    private:
      bool outfdeof[2];
      bool stdouteof;
      bool system_;
      uid_t uid_;
      gid_t gid_;
      int timeout_;
      bool timed_out;
      int code_;
      int statefd[2];
      int infd[2];
      int outfd[2][2];
      pid_t pid_;
      std::string command_;
      time_t start_time;
  };

}

#endif
