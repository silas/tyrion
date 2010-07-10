/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include "node_process.h"

#include <cstdlib>
#include <sys/wait.h>
#include "utils.h"

namespace tyrion {

NodeProcess::NodeProcess(std::string command, bool system, int timeout) {
  command_ = command;
  system_ = system;
  timed_out_ = false;
  ran_ = false;

  pipe(infd);
  pipe(outfd[0]);
  pipe(outfd[1]);

  outfdeof[0] = false;
  outfdeof[1] = false;

  timeout_ = timeout > 0 ? timeout : 1;

  uid_ = getuid();
  gid_ = getgid();
}

NodeProcess::~NodeProcess() {
  if (ran_) {
    close(infd[1]);
    close(outfd[0][0]);
    close(outfd[1][0]);
  }
}

void NodeProcess::Run() {
  if (ran_)
    return;
  ran_ = true;

  pid_ = fork();

  time(&start_time);

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

void NodeProcess::Write(std::string text, bool eof) {
  if (!text.empty())
    write(infd[1], text.c_str(), text.length());

  if (eof)
    close(infd[1]);
}

bool NodeProcess::Done() {
  return (outfdeof[Stdout] && outfdeof[Stderr]) || TimedOut();
}

bool NodeProcess::TimedOut() {
  if (timed_out_)
    return true;

  time_t current_time;
  time(&current_time);
  timed_out_ = difftime(current_time, start_time) >= timeout_;

  return timed_out_;
}

int NodeProcess::Close() {
  int state = -1;
  int rc = 0;

  for (int i = 0; i < 10 && rc <= 0; i++) {
    rc = waitpid(pid_, &state, WNOHANG);
    if (TimedOut())
      break;
    usleep(i * 100000);
  }

  if (rc > 0) {
    return WEXITSTATUS(state);
  } else {
    kill(pid_, SIGTERM);
  }

  for (int i = 0; i < 10 && rc <= 0; i++) {
    rc = waitpid(pid_, &state, WNOHANG);
    usleep(i * 100000);
  }

  if (rc > 0) {
    return 15;
  } else {
    kill(pid_, SIGKILL);
  }

  return 137;
}

bool NodeProcess::set_user(std::string name, bool set_group) {
  struct passwd* pw = getpwnam(name.c_str());

  if (pw) {
    uid_ = pw->pw_uid;

    if (set_group)
      gid_ = pw->pw_gid;

    return true;
  }

  return false;
}

bool NodeProcess::set_group(std::string name) {
  struct group* gr = getgrnam(name.c_str());

  if (gr) {
    gid_ = gr->gr_gid;
    return true;
  }

  return false;
}

}  // namespace tyrion
