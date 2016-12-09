//
//File: E_Wizard.h
//Author: Ivlev Alexander. Stef
//Created: 27/11/2016
//

#pragma once

#include "model/Wizard.h"
#include "model/ActionType.h"
#include "R_Role.h"
#include <memory>

namespace AICup
{
  class Wizard: public model::Wizard {
  public:
    Wizard(const model::Wizard& wizard, const Role& role);

    bool availableAction(const model::ActionType action) const;

    int minAttackCooldown() const;
    int minStaffOrMissileCooldown() const;

    int cooldown(const model::ActionType action) const;
    bool isCooldown(const model::ActionType action) const;

    double maxSpeed() const;
    double maxBackwardSpeed() const;
    double maxTurnSpeed() const;

    double damage(const model::ActionType action) const;
    double dps(const model::ActionType action) const;

    bool isBurn() const;
    double burnResidualDamage() const;

    /// Вообще я бы её отдельно посылал, но уже менять тяжко, поэтому сюда впихнул
    inline const Role& getRole() const {
      return role;
    }

  private:
    const Role& role;
  };
}

