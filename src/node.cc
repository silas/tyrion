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
#include <iostream>
#include <txmpp/cryptstring.h>
#include <txmpp/logging.h>
#include <txmpp/xmppclientsettings.h>
#include "xmppthread.h"

int main(int argc, char* argv[]) {

  int exit_code = 0;
  int sig;
  sigset_t set;

  sigemptyset(&set);
  sigaddset(&set, SIGHUP);
  sigaddset(&set, SIGINT);
  sigaddset(&set, SIGTERM);
  sigaddset(&set, SIGUSR2);
  pthread_sigmask(SIG_BLOCK, &set, NULL);

  bool reconnect = true;

  txmpp::LogMessage::LogToDebug(txmpp::LS_SENSITIVE);

  txmpp::InsecureCryptStringImpl password;
  password.password() = "test";

  while (reconnect) {
    exit_code = 0;

    // Start xmpp on a different thread
    tyrion::XmppThread thread;
    thread.Start();

    // Create client settings
    txmpp::XmppClientSettings xcs;
    xcs.set_user("test");
    xcs.set_pass(txmpp::CryptString(password));
    xcs.set_host("example.org");
    xcs.set_resource("resource");
    xcs.set_use_tls(true);
    xcs.set_server(txmpp::SocketAddress("example.org", 5222));

    thread.Login(xcs);

    while (true) {
      sigwait(&set, &sig);

      if (sig == SIGUSR2) {
        if (thread.shutdown()) reconnect = false;
        exit_code = thread.exit_code();
        break;
      } else if (sig == SIGHUP) {
        thread.Disconnect();
        break;
      } else if (sig == SIGINT || sig == SIGTERM) {
        reconnect = false;
        thread.Disconnect();
        break;
      }
    }

    thread.Quit();
    thread.Stop();
  }

  std::cout << "Exiting..." << std::endl;

  return exit_code;
}
