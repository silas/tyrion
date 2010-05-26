/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef TYRION_QUEUE_H_
#define TYRION_QUEUE_H_

#include <queue>
#include <pthread.h>

namespace tyrion
{
  template <class T>
  class Queue
  {
    public:
      Queue()
      {
        pthread_mutex_init(&mutex_, NULL);
      }

      ~Queue()
      {
        pthread_mutex_destroy(&mutex_);
      }

      void Push(T value)
      {
        pthread_mutex_lock(&mutex_);
        queue_.push(value);
        pthread_mutex_unlock(&mutex_);
      }

      T Pop()
      {
        pthread_mutex_lock(&mutex_);
        T value = queue_.front();
        queue_.pop();
        pthread_mutex_unlock(&mutex_);
        return value;
      }

      bool Empty()
      {
        return queue_.empty();
      }

      bool Size()
      {
        return queue_.size();
      }

    private:
      pthread_mutex_t mutex_;
      std::queue<T> queue_;
  };

}

#endif
