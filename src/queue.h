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

#include <pthread.h>
#include <queue>

namespace tyrion
{
  template <class T>
  class Queue
  {
    public:
      Queue()
      {
        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&cond, NULL);
      }

      ~Queue()
      {
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond);
      }

      void Push(T value)
      {
        pthread_mutex_lock(&mutex);
        queue.push(value);
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
      }

      T Pop()
      {
        pthread_mutex_lock(&mutex);
        if (queue.size() < 1)
            pthread_cond_wait(&cond, &mutex);
        T value = queue.front();
        queue.pop();
        pthread_mutex_unlock(&mutex);
        return value;
      }

      bool Empty()
      {
        return queue.empty();
      }

      bool Size()
      {
        return queue.size();
      }

    private:
      pthread_mutex_t mutex;
      pthread_cond_t cond;
      std::queue<T> queue;
  };

}

#endif
