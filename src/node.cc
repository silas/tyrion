/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include <csignal>
#include <pthread.h>
#include "node_loop.h"
#include "node_settings.h"
#include "node_utils.h"
#ifdef _DEBUG
#include <txmpp/logging.h>
#endif

int main(int argc, char* argv[]) {

#ifdef _DEBUG
  txmpp::LogMessage::LogToDebug(txmpp::LS_SENSITIVE);
#endif

  int code = 0;
  int sig;
  sigset_t set;

  sigemptyset(&set);
  sigaddset(&set, SIGHUP);
  sigaddset(&set, SIGUSR1);
  sigaddset(&set, SIGINT);
  sigaddset(&set, SIGTERM);
  pthread_sigmask(SIG_BLOCK, &set, NULL);

  tyrion::Logging::Instance()->Debug(tyrion::Logging::INFO);
  tyrion::NodeSetup(argc, argv);

  tyrion::NodeLoop* loop = tyrion::NodeLoop::Instance();
  loop->set_pthread(pthread_self());
  loop->Start();
  loop->Login();

  while (true) {
    sigwait(&set, &sig);

    if (sig == SIGHUP) {
      loop->Reload();
    } else if (sig == SIGUSR1) {
      tyrion::NodeReloadLogging();
    } else if (sig == SIGINT || sig == SIGTERM) {
      if (loop->state() == tyrion::NodeLoop::ERROR) {
        code = 1;
      } else if (loop->state() == tyrion::NodeLoop::RUNNING) {
        loop->Disconnect();
      }
      loop->Quit();
      loop->Stop();
      break;
    }
  }

  delete loop;
  tyrion::NodeExit(code);
}
