//
//File: CM_CommandFabric.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#pragma once

#include "CM_MoveCommand.h"
#include "CM_AttackCommand.h"
#include "E_Types.h"
#include "model\LaneType.h"

namespace AICup
{
  class CommandFabric {
  public:
    CommandFabric(Algorithm::PathFinder& finder);

/// #pragma mark - �����������
    /// ��������� �� ����� �� ������������ ����������
    MoveCommandPtr keepDistance(const double x, const double y, const double minDistance, const double maxDistance) const;

    /// ������������� � �����
    MoveCommandPtr moveToPoint(const double x, const double y, const TurnStyle style = TurnStyle::TURN, const double speedLimit = -1) const;

    /// ������������� �� �����
    MoveCommandPtr moveToLine(const model::LaneType line) const;

    /// ��������� �� ������
    MoveCommandPtr follow(const long long unitId, const double minDistance = 0, const double maxDistance = 10000) const;

    /// ������� �� ���������� �����
    MoveCommandPtr moveGetExpirience() const;

    /// ������� ����� ����� ����
    MoveCommandPtr moveToBonus() const;

    /// ������������ ����� - ��������� ���� ��� ��� �����
    MoveCommandPtr observeMap() const;

/// #pragma mark - �����
    /// ��������� �����
    AttackCommandPtr attack(const model::LivingUnit& unit) const;

    /// ��� ����������� ������
    AttackCommandPtr pool(const long long neutralUnitId) const;


/// #pragma mark - ������
    /// �������� �������
    MoveCommandPtr defend(const double x, const double y) const;

    /// ��������� �� ���������� ���������� �� �����
    MoveCommandPtr avoidEnemy(const long long unitId) const;

  private:
    Algorithm::PathFinder& finder;
  };
}
