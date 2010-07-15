/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include <csignal>
#include <pthread.h>
#ifdef _DEBUG
#include <txmpp/logging.h>
#endif
#include "node_loop.h"
#include "node_service_handler.h"
#include "node_settings.h"
#include "node_utils.h"

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

  tyrion::NodeServiceHandler* service_handler = new tyrion::NodeServiceHandler();
  tyrion::NodeLoop* loop = new tyrion::NodeLoop(pthread_self());

  code = tyrion::NodeSetup(argc, argv, loop, false);
  if (code == 0) {
    loop->set_service_handler(service_handler);
  } else {
    delete service_handler;
    tyrion::NodeExit(code);
  }

  loop->Start();
  loop->Login();
  loop->service_handler()->Start();

  while (true) {
    sigwait(&set, &sig);

    if (sig == SIGHUP) {
      loop = NodeReload(argc, argv, loop, service_handler);
    } else if (sig == SIGUSR1) {
      if (tyrion::NodeSetupLogging(loop->settings(), true)) {
        TLOG(INFO) << "Logging reloaded.";
      } else {
        TLOG(INFO) << "Unable to reload logging.";
      }
    } else if (sig == SIGINT || sig == SIGTERM) {
      if (loop->state() == tyrion::NodeLoop::ERROR) {
        code = 1;
      } else if (loop->state() == tyrion::NodeLoop::RUNNING) {
        loop->Disconnect();
      }
      loop->Quit();
      loop->Stop();
      loop->service_handler()->Quit();
      loop->service_handler()->Stop();
      break;
    }
  }

  delete loop;
  tyrion::NodeExit(code);
}
