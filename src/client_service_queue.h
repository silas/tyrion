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

namespace tyrion {
namespace client {

class ServiceQueueItem {
  public:
    enum ServiceQueueItemNotification {
      None,
      Disconnect
    };

    enum ServiceQueueItemState {
      ServiceUnavailable
    };

    // Constructs an item which can be used to send notifications such as
    // Disconnect.
    ServiceQueueItem(ServiceQueueItemNotification notification = None) {
      set_notification(notification);
    }
    // Constructs an item for a service response which didn't have service
    // element.
    ServiceQueueItem(std::string jid, std::string id = "") {
      set_jid(jid);
      set_id(id);
      set_notification(None);
      set_state(ServiceUnavailable);
    }
    // Constructs a valid service response item.
    ServiceQueueItem(std::string jid, XmppService service,
                     std::string id = "") {
      set_jid(jid);
      set_service(service);
      set_id(id);
      set_notification(None);
    }

    std::string id() {
      return id_;
    }
    void set_id(std::string id) {
      id_ = id;
    }

    std::string jid() {
      return jid_;
    }
    void set_jid(std::string jid) {
      jid_ = jid;
    }

    ServiceQueueItemNotification notification() {
      return notification_;
    }
    void set_notification(ServiceQueueItemNotification notification) {
      notification_ = notification;
    }

    XmppService service() {
      return service_;
    }
    void set_service(XmppService service) {
      service_ = service;
    }

    ServiceQueueItemState state() {
      return state_;
    }
    void set_state(ServiceQueueItemState state) {
      state_ = state;
    }

  private:
    std::string id_;
    std::string jid_;
    ServiceQueueItemNotification notification_;
    XmppService service_;
    ServiceQueueItemState state_;
};

// Queue used to pass service requests and responses between the CLI
// application and the XMPP client thread.
typedef tyrion::Queue<ServiceQueueItem> ServiceQueue;

} }  // namespace tyrion::client

#endif  // TYRION_CLIENT_SERVICE_QUEUE_H_
