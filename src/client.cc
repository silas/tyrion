/*
 * Copyright (c) 2010, The Infrastructure Developers Group
 * All rights reserved.
 *
 * This file is subject to the New BSD License (see the LICENSE file).
 */

#include <csignal>
#include <pthread.h>
#include "client_loop.h"
#include "client_request.h"
#include "client_settings.h"
#include "client_utils.h"
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
  sigaddset(&set, SIGINT);
  sigaddset(&set, SIGTERM);
  pthread_sigmask(SIG_BLOCK, &set, NULL);

  tyrion::ClientLoop* loop = new tyrion::ClientLoop(pthread_self());
  tyrion::ClientRequest request(loop);

  tyrion::Logging::Instance()->Debug(tyrion::Logging::WARNING);
  tyrion::ClientSetup(argc, argv, &request);

  loop->Start();
  loop->Login();

  std::string data, x;
  while (!std::cin.eof()) {
    std::getline(std::cin, x);
    data += x + '\n';
  }
  request.set_input(data);

  tyrion::StringList* jid = request.jid();
  for(tyrion::StringList::iterator it = jid->begin(); it != jid->end(); ++it) {
    loop->Request(request.CreateClientEnvelope(*it));
  }

  while (true) {
    sigwait(&set, &sig);

    if (sig == SIGHUP || sig == SIGINT || sig == SIGTERM) {
      if (loop->state() == tyrion::ClientLoop::ERROR) {
        code = 1;
      } else if (loop->state() == tyrion::ClientLoop::RUNNING) {
        loop->Disconnect();
      }
      loop->Quit();
      loop->Stop();
      break;
    }
  }

  delete loop;
  tyrion::ClientExit(code);
}
