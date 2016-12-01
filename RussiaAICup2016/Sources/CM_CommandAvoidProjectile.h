//
//File: CM_CommandAvoidProjectile.h
//Author: Ivlev Alexander. Stef
//Created: 29/11/2016
//


#pragma once

#include "CM_MoveCommand.h"
#include "model\Projectile.h"

namespace AICup
{
  class CommandAvoidProjectile: public MoveCommand {
  public:
    CommandAvoidProjectile(const model::Projectile& projectile);

    bool check(const Wizard& self) override;

    void execute(const Wizard& self, Result& result) override;

    double priority(const Wizard& self) override;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const model::Wizard& self, const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR

  private:
    const model::Projectile& projectile;

    TurnStyle turnStyle;
    Position position;
  };
}