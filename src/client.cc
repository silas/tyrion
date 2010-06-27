/*
 * Tyrion
 * Copyright 2010, Silas Sewell
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

  tyrion::ClientRequest request;

  tyrion::Logging::Instance()->Debug(tyrion::Logging::WARNING);
  tyrion::ClientSetup(argc, argv, &request);

  tyrion::ClientLoop* loop = tyrion::ClientLoop::Instance();
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
      if (loop->state() == tyrion::ClientLoop::ERROR) code = 1;
      loop->Disconnect();
      loop->Quit();
      loop->Stop();
      break;
    }
  }

  delete loop;
  tyrion::ClientExit(0);
}
