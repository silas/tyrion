/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef TYRION_CLIENT_SERVICE_QUEUE_H_
#define TYRION_CLIENT_SERVICE_QUEUE_H_

#include "queue.h"
#include "xmpp_service.h"

namespace tyrion
{
  namespace client
  {

    class ServiceQueueItem
    {
      public:
        enum ServiceQueueItemNotification
        {
          None,
          Disconnect
        };
        enum ServiceQueueItemState
        {
          ServiceUnavailable
        };
        ServiceQueueItem(ServiceQueueItemNotification notification = None)
          : notification_(notification) {}
        ServiceQueueItem(std::string jid, std::string id = "")
          : jid_(jid), id_(id), notification_(None), state_(ServiceUnavailable) {}
        ServiceQueueItem(std::string jid, XmppService service, std::string id = "")
          : jid_(jid), service_(service), id_(id), notification_(None) {}
        std::string GetJid() { return jid_; }
        std::string GetId() { return id_; }
        XmppService GetService() { return service_; }
        ServiceQueueItemNotification GetNotification() { return notification_; }
        ServiceQueueItemState GetState() { return state_; }
        void SetJid(std::string jid) { jid_ = jid; }
        void SetId(std::string id) { id_ = id; }
        void SetNotification(ServiceQueueItemNotification notification) { notification_ = notification; }
        void SetState(ServiceQueueItemState state) { state_ = state; }
        void SetService(XmppService service) { service_ = service; }

      private:
        std::string jid_;
        std::string id_;
        XmppService service_;
        ServiceQueueItemNotification notification_;
        ServiceQueueItemState state_;
    };

    typedef tyrion::Queue<ServiceQueueItem> ServiceQueue;
  }
}

#endif
