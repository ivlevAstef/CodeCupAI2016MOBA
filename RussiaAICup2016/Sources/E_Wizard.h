//
//File: E_Wizard.h
//Author: Ivlev Alexander. Stef
//Created: 27/11/2016
//

#pragma once

#include "model/Wizard.h"
#include "R_Role.h"
#include <memory>

namespace AICup
{
  class Wizard: public model::Wizard {
  public:

  public:
    Wizard(const model::Wizard& wizard);
    void moveInfoFrom(const Wizard& last);

    /// читает message, и кидает обновление под классам
    void update(model::Move& move);

    inline const Role& getRole() const {
      return *role.get();
    }

  private:
    std::shared_ptr<Role> role;
  };
}

