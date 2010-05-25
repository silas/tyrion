/*
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org/

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "node_acl.h"

namespace tyrion
{
  namespace node
  {

    Acl* Acl::instance_ = NULL;

    Acl* Acl::Instance()
    {
      if (!instance_)
        instance_ = new Acl;

      return instance_;
    }

  }
}
