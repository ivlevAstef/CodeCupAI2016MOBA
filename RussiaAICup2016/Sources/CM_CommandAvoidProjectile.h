//
//File: CM_CommandAvoidProjectile.h
//Author: Ivlev Alexander. Stef
//Created: 29/11/2016
//


#pragma once

#include "CM_MoveCommand.h"
#include "E_Bullet.h"

namespace AICup
{
  class CommandAvoidProjectile: public MoveCommand {
  public:
    CommandAvoidProjectile(const Bullet& projectile, const Vector& moveDirection);

    bool check(const Wizard& self) override;

    void execute(const Wizard& self, Result& result) override;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const model::Wizard& self, const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR

  private:
    const Bullet& projectile;
    const Vector& moveDirection;

    Vector turnDirection;
    Position position;
  };
}