//
//File: CM_CommandFabric.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#pragma once

#include "CM_MoveCommand.h"
#include "CM_AttackCommand.h"
#include "CM_CastCommand.h"
#include "E_Types.h"
#include "model\LaneType.h"
#include "model\Projectile.h"
#include "model\Minion.h"

namespace AICup
{
  class CommandFabric {
  public:
    CommandFabric(Algorithm::PathFinder& finder);

/// #pragma mark - перемещение
    /// даржаться от точки на определенном расстоянии
    MoveCommandPtr keepDistance(const double x, const double y, const double minDistance, const double maxDistance) const;

    /// переместиться в точку
    MoveCommandPtr moveToPoint(const double x, const double y, const TurnStyle style = TurnStyle::TURN, const double speedLimit = -1) const;

    /// переместиться на линию
    MoveCommandPtr moveToLine(const model::LaneType line) const;

    /// следовать за юнитом
    MoveCommandPtr follow(const long long unitId, const double minDistance = 0, const double maxDistance = 10000) const;

    /// подойти за получением опыта
    MoveCommandPtr moveGetExpirience() const;

    /// подойти чтобы взять руну
    MoveCommandPtr moveToBonus() const;

    /// обследование карты - посещение мест где был давно
    MoveCommandPtr observeMap() const;

/// #pragma mark - атака
    MoveCommandPtr followAttack(const model::Wizard& wizard) const;

    /// атаковать врага
    AttackCommandPtr attack(const model::LivingUnit& unit) const;

    AttackCommandPtr attackUseFrostbolt() const;
    AttackCommandPtr attackUseFireball() const;

    /// пул нейтральных крипов
    AttackCommandPtr pool(const long long neutralUnitId) const;


/// #pragma mark - защита
    MoveCommandPtr pushOffMinion(const model::Minion& minion) const;

    /// держаться на безопасном расстоянии от юнита
    MoveCommandPtr avoidEnemy(const model::LivingUnit& unit) const;

    // /уворот от снаряда
    MoveCommandPtr avoidProjectile(const model::Projectile& projectile) const;

/// #pragma mark - cast
    CastCommandPtr haste() const;
    CastCommandPtr shield() const;

  private:
    Algorithm::PathFinder& finder;
  };
}
