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
    Singleton() {
      assert(nullptr == ref());
      ref(static_cast<Parent*>(this));
    }

    static Parent& instance() {
      assert(nullptr != ref());
      return *ref();
    }

  private:
    Singleton(const Singleton<Parent>&) {
      assert(true);
    }
    Singleton<Parent>& operator=(const Singleton<Parent>&) {
      assert(true);
      return *this;
    }

  private:
    static Parent* ref(Parent* pThis = nullptr) {
      static Parent* sSingleton = nullptr;
      if (nullptr != pThis) {
        sSingleton = pThis;
      }
      return sSingleton;
    }
  };
};
