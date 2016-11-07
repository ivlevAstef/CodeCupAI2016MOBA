//
//File: Singleton.h
//Description:
//Author: Ivlev Alexander. Stef
//Created: 21:21 16/7/2015
//Copyright (c) SIA 2015. All Right Reserved.
//

/*Author: Ivlev Alexander (stef).*/
#pragma once

#include <assert.h>

namespace AICup
{
  template<typename Parent>
  class Singleton
  {
  public:
    static Parent& instance() {
      return *ref();
    }

  private:
    Singleton() {
      assert(true);
    }

    Singleton(const Singleton<Parent>&) {
      assert(true);
    }
    Singleton<Parent>& operator=(const Singleton<Parent>&) {
      assert(true);
      return *this;
    }

  private:
    static Parent* ref() {
      static Parent* sSingleton = Parent();
      return sSingleton;
    }
  };
};
