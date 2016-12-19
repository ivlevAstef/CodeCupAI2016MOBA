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
#include "E_Bullet.h"
#include "model\Minion.h"

namespace AICup
{
  class CommandFabric {
  public:
    CommandFabric();

/// #pragma mark - �����������
    /// ������������� � �����
    MoveCommandPtr moveToPoint(const double x, const double y, const double priority) const;

    /// ������������� �� �����
    MoveCommandPtr moveToLine(const model::LaneType line) const;

    /// ��������� �� ������
    MoveCommandPtr follow(const long long unitId, const double minDistance = 0, const double maxDistance = 10000) const;

    /// ������� �� ���������� �����
    MoveCommandPtr moveGetExpirience() const;

    /// ������� ����� ����� ����
    MoveCommandPtr moveToBonus() const;

/// #pragma mark - �����
    MoveCommandPtr moveMeleeAttack(const model::LivingUnit& unit) const;

    MoveCommandPtr followAttack(const model::Wizard& wizard) const;

    /// ��������� �����
    AttackCommandPtr attack(const model::LivingUnit& unit) const;

    AttackCommandPtr attackUseFrostbolt() const;
    AttackCommandPtr attackUseFireball() const;

    /// ��� ����������� ������
    AttackCommandPtr pool(const long long neutralUnitId) const;


/// #pragma mark - ������

    MoveCommandPtr repulsionForDodgeFireBall() const;

    /// ��������� �� ���������� ���������� �� �����
    MoveCommandPtr avoidEnemy(const model::LivingUnit& unit) const;

    // /������ �� �������
    MoveCommandPtr avoidProjectile(const Bullet& projectile, const Vector& moveDirection) const;

/// #pragma mark - cast
    CastCommandPtr haste() const;
    CastCommandPtr shield() const;
  };
}
