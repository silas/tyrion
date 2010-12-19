/*
 * Copyright (c) 2010, Silas Sewell
 * All rights reserved.
 *
 * This file is subject to the MIT License (see the LICENSE file).
 */

#include "process.h"

#include <cstdlib>
#include <grp.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <iostream>
#include "utils.h"

namespace tyrion {

Process::Process(std::string command, bool system, int timeout) :
    pid_(0),
    uid_(0),
    gid_(0),
    command_(command),
    system_(system),
    start_time_(NULL),
    timed_out_(false),
    timeout_(PROCESS_TIMEOUT),
    code_(0),
    ran_(false),
    closed_(false) {
  pipe(infd);
  pipe(outfd[0]);
  pipe(outfd[1]);

  outfdeof[0] = false;
  outfdeof[1] = false;

  timeout_ = timeout > 0 ? timeout : 1;

  uid_ = getuid();
  gid_ = getgid();
}

Process::~Process() {
  if (ran_) {
    close(infd[1]);
    close(outfd[0][0]);
    close(outfd[1][0]);
  }
}

void Process::Run() {
  if (ran_)
    return;
  ran_ = true;

  pid_ = fork();

  time(&start_time_);

  if(pid_ == 0) {
    bool issue = false;

    close(STDOUT_FILENO);
    close(STDIN_FILENO);
    close(STDERR_FILENO);

    dup2(infd[0], STDIN_FILENO);
    dup2(outfd[0][1], STDOUT_FILENO);
    dup2(outfd[1][1], STDERR_FILENO);

    close(infd[0]);
    close(infd[1]);
    close(outfd[0][0]);
    close(outfd[0][1]);
    close(outfd[1][0]);
    close(outfd[1][1]);

    if (!issue) {
      int sg = setgid(gid_);

      if (sg != 0) {
        std::cerr << CreateError("group.set", "Unable to set gid");
        issue = true;
      }
    }

    if (!issue) {
      int su = setuid(uid_);

      if (su != 0) {
        std::cerr << CreateError("user.set", "Unable to set uid");
        issue = true;
      }
    }

    if (issue) {
      execlp(PROCESS_ISSUE_COMMAND, PROCESS_ISSUE_COMMAND, NULL);
    } else if (system_) {
      int code = system(command_.c_str());
      exit(WEXITSTATUS(code));
    } else {
      execl(command_.c_str(), command_.c_str(), NULL);
    }

    exit(EXIT_SUCCESS);
  } else if (pid_ > 0) {
    close(infd[0]);
    close(outfd[0][1]);
    close(outfd[1][1]);
  }
}

void Process::Write(std::string text, bool eof) {
  if (!text.empty())
    write(infd[1], text.c_str(), text.length());

  if (eof)
    close(infd[1]);
}

bool Process::Done() {
  return (outfdeof[Stdout] && outfdeof[Stderr]) || TimedOut();
}

bool Process::TimedOut() {
  if (closed_ || timed_out_)
    return timed_out_;

  time_t current_time;
  time(&current_time);
  timed_out_ = difftime(current_time, start_time_) >= timeout_;

  return timed_out_;
}

int Process::Close() {
  if (closed_)
    return code_;
  closed_ = true;
  int state = -1;
  int rc = 0;

  for (int i = 0; i < 10 && rc <= 0; i++) {
    rc = waitpid(pid_, &state, WNOHANG);
    if (TimedOut())
      break;
    usleep(i * 100000);
  }

  if (rc > 0) {
    return code_ = WEXITSTATUS(state);
  } else {
    kill(pid_, SIGTERM);
  }

  for (int i = 0; i < 10 && rc <= 0; i++) {
    rc = waitpid(pid_, &state, WNOHANG);
    usleep(i * 100000);
  }

  if (rc > 0) {
    return code_ = 15;
  } else {
    kill(pid_, SIGKILL);
  }

  return code_ = 137;
}

bool Process::set_user(std::string name, bool set_group) {
  struct passwd* pw = getpwnam(name.c_str());

  if (pw) {
    uid_ = pw->pw_uid;

    if (set_group)
      gid_ = pw->pw_gid;

    return true;
  }

  return false;
}

bool Process::set_group(std::string name) {
  struct group* gr = getgrnam(name.c_str());

  if (gr) {
    gid_ = gr->gr_gid;
    return true;
  }

  return false;
}

}  // namespace tyrion
