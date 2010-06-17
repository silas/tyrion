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
#include <txmpp/cryptstring.h>
#include <txmpp/thread.h>
#include <txmpp/xmppclientsettings.h>
#include "logging.h"
#include "node_settings.h"
#include "node_utils.h"
#include "xmppthread.h"
#ifdef _DEBUG
#include <txmpp/logging.h>
#endif

int main(int argc, char* argv[]) {

#ifdef _DEBUG
  txmpp::LogMessage::LogToDebug(txmpp::LS_SENSITIVE);
#endif

  sigset_t set;
  sigset_t set_waiting;

  sigemptyset(&set);
  sigaddset(&set, SIGHUP);
  sigaddset(&set, SIGINT);
  sigaddset(&set, SIGTERM);
  sigaddset(&set, SIGUSR2);
  pthread_sigmask(SIG_BLOCK, &set, NULL);

  tyrion::Logging::Instance()->Debug(tyrion::Logging::DEBUG);
  tyrion::utils::SetupConfig(argc, argv);

  bool reconnect = true;
  int code, sig, timeout = 0;

  while (reconnect) {
    code = 0;

    tyrion::XmppThread thread;
    thread.Start();

    txmpp::Jid jid(tyrion::NodeSettings::Instance()->Get("xmpp", "jid"));

    txmpp::InsecureCryptStringImpl password;
    password.password() = tyrion::NodeSettings::Instance()->Get("xmpp", "password");

    txmpp::XmppClientSettings settings;
    settings.set_user(jid.node());
    settings.set_pass(txmpp::CryptString(password));
    settings.set_host(jid.domain());
    settings.set_resource(jid.resource());
    settings.set_use_tls(true);

    settings.set_server(txmpp::SocketAddress(
        tyrion::NodeSettings::Instance()->Has("xmpp", "server") ?
            tyrion::NodeSettings::Instance()->Get("xmpp", "server") : jid.domain(),
        tyrion::NodeSettings::Instance()->GetInt("xmpp", "port", 5222)
    ));

    if (timeout > 0)
      TLOG(INFO) << "Reconnecting in " << timeout / 1000 << " seconds...";

    if (reconnect) {
      thread.Login(settings, timeout);

      while (true) {
        sigwait(&set, &sig);

        if (sig == SIGUSR2) {
          if (thread.state() >= tyrion::XmppThread::SHUTDOWN)
            reconnect = false;
          if (thread.state() >= tyrion::XmppThread::SHUTDOWN_ERROR)
            code = 1;
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
    }

    thread.Quit();
    thread.Stop();

    if (timeout < 1) timeout = 10000;
  }

  return code;
}
