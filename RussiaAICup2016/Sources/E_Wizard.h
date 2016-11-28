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

    bool availableAction(const model::ActionType action) const;
    int minAttackCooldown() const;
    int minStaffOrMissileCooldown() const;
    int cooldown(const model::ActionType action) const;
    bool isCooldown(const model::ActionType action) const;
    double maxSpeed() const;
    double damage(const model::ActionType action) const;
    double dps(const model::ActionType action) const;

    inline const Role& getRole() const {
      return *role.get();
    }

  private:
    std::shared_ptr<Role> role;
  };
}

