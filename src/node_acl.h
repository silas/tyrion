/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef TYRION_NODE_ACL_H_
#define TYRION_NODE_ACL_H_

#include <iostream>
#include "setting.h"

namespace tyrion
{
  namespace node
  {

    class Acl : public BaseSetting
    {
      public:
        static Acl* Instance();

      private:
        static Acl* instance_;
    };

  }
}

#endif
