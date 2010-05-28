/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "process.h"

#include <cstdlib>
#include <sys/wait.h>
#include "tyrion.h"
#include "utils.h"

#define PROCESS_BUFFER 1024
#define PROCESS_BUFFER_SLEEP 100000
#define PROCESS_ISSUE_COMMAND "false"

namespace tyrion {

Process::Process(std::string command, bool system, int timeout) {
  command_ = command;
  system_ = system;
  timed_out_ = false;

  pipe(infd);
  pipe(outfd[0]);
  pipe(outfd[1]);

  fcntl(outfd[0][0], F_SETFL, O_NONBLOCK);
  fcntl(outfd[1][0], F_SETFL, O_NONBLOCK);

  outfdeof[0] = false;
  outfdeof[1] = false;

  timeout_ = timeout > 0 ? timeout : 1;

  uid_ = getuid();
  gid_ = getgid();
}

Process::~Process() {
  close(infd[1]);
  close(outfd[0][0]);
  close(outfd[1][0]);
}

void Process::Run() {
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
        std::cerr << utils::Error("group.set", "Unable to set gid");
        issue = true;
      }
    }

    if (!issue) {
      int su = setuid(uid_);

      if (su != 0) {
        std::cerr << utils::Error("user.set", "Unable to set uid");
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

std::string Process::Read(ProcessType type) {
  char input[PROCESS_BUFFER];
  int rc = read(outfd[type][0], input, PROCESS_BUFFER-1);

  if (rc == 0) {
    outfdeof[type] = true;
  } else if (rc == -1) {
    usleep(PROCESS_BUFFER_SLEEP);
  } else {
    input[rc] = 0;
    return std::string(input);
  }

  return "";
}

std::string Process::ReadAll(ProcessType type) {
  std::string data;
  std::string buffer;

  while(!Empty(type)) {
    buffer = Read(type);

    if (!buffer.empty())
      data += buffer;

    if (TimedOut())
      break;
  }

  return data;
}

bool Process::Empty(ProcessType type) {
  return outfdeof[type];
}

void Process::Write(std::string text) {
  if (!text.empty())
    write(infd[1], text.c_str(), text.length());
}

bool Process::TimedOut() {
  if (timed_out_)
    return true;

  time_t current_time;
  time(&current_time);
  timed_out_ = difftime(current_time, start_time) >= timeout_;

  return timed_out_;
}

void Process::Eof() {
  close(infd[1]);
}

int Process::Close() {
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

} // namespace tyrion
