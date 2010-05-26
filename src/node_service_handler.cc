/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "node_service_handler.h"

#include <gloox/error.h>
#include "error.h"
#include "process.h"
#include "node_acl.h"
#include "setting.h"
#include "tyrion.h"

namespace tyrion
{
  namespace node
  {

    ServiceHandler::ServiceHandler(
      gloox::ClientBase* client,
      gloox::JID jid,
      std::string id,
      std::string type,
      std::string input,
      std::string path)
    {
      client_ = client;
      jid_ = jid;
      id_ = id;
      type_ = type;
      input_ = input;
      path_ = path;
      timeout_ = SERVICE_TIMEOUT;
    }

    void ServiceHandler::Run()
    {
      if (Acl::Instance()->GetBool(type_, jid_.bare())
        || Acl::Instance()->GetBool(type_, jid_.full()))
      {
        bool issue = false;
        std::string output;
        std::string error;

        std::string config = "service:" + type_;
        std::string user = Setting::Instance()->Get(config, "user");
        std::string group = Setting::Instance()->Get(config, "group");
        int timeout = Setting::Instance()->GetInt(config, "timeout");
        bool enforce_user = Setting::Instance()->GetBool(config, "enforce_user");
        bool enforce_group = Setting::Instance()->GetBool(config, "enforce_group");
        bool enforce_timeout = Setting::Instance()->GetBool(config, "enforce_timeout");

        if (!issue && enforce_user)
        {
          if (user.empty())
          {
            error += Error::Create("user.enforce", "Unable to enforce user because none set.");
            issue = true;
          }
        }
        else if (!issue && !user_.empty())
          user = user_;

        if (!issue && enforce_group)
        {
          if (group.empty())
          {
            error += Error::Create("group.enforce", "Unable to enforce group because none set.");
            issue = true;
          }
        }
        else if (!issue && !group_.empty())
          group = group_;

        if (!issue && enforce_timeout)
        {
          if (!timeout > 0)
          {
            error += Error::Create("timeout.enforce", "Unable to enforce timeout because none set.");
            issue = true;
          }
        }
        else if (!issue && timeout_ > 0)
          timeout = timeout_;
        else
          timeout = SERVICE_TIMEOUT;

        Process p = Process(path_, false, timeout);

        if (!issue && !user.empty() && !p.SetUser(user))
        {
          error += Error::Create("user.lookup", "Unable to find user '" + user
            + "'");
          issue = true;
        }

        if (!issue && !group.empty() && !p.SetGroup(group))
        {
          error += Error::Create("group.lookup", "Unable to find group '" + group
            + "'");
          issue = true;
        }

        int code = -1;

        if (!issue)
        {
          p.Run();
          p.Write(input_);
          p.Eof();

          output += p.ReadAll(Process::Stdout);
          error += p.ReadAll(Process::Stderr);
          code = p.Close();
        }

        XmppService *service = new XmppService();
        service->SetOutput(output);
        service->SetError(error);
        service->SetType(type_);
        service->SetCode(code);

        gloox::IQ re(gloox::IQ::Result, jid_, id_);
        re.addExtension(service);
        client_->send(re);
      }
      else
      {
        gloox::IQ re(gloox::IQ::Error, jid_, id_);
        re.addExtension(
          new gloox::Error(
            gloox::StanzaErrorTypeCancel,
            gloox::StanzaErrorServiceUnavailable
          )
        );

        client_->send(re);
      }
    }

  }
}
