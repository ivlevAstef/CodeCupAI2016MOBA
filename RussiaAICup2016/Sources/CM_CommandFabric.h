//
//File: CM_CommandFabric.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#pragma once

#include "CM_Command.h"
#include "model\LaneType.h"
#include <initializer_list>

namespace AICup
{
  class CommandFabric {
  public:
#pragma mark - �����������
    /// ������������� � �����
    CommandPtr moveToPoint(const double x, const double y) const;

    /// ������������� �� �����
    CommandPtr moveToLine(const model::LaneType line) const;

    /// ��������� �� ������
    CommandPtr follow(const long long unitId, const double minDistance = 0, const double maxDistance = 10000) const;

    /// ������������ ����� - ��������� ���� ��� ��� �����
    CommandPtr observeMap() const;

#pragma mark - �����
    /// ��������� �����
    CommandPtr attack(const long long enemyId) const;

    /// ��� ����������� ������
    CommandPtr pool(const long long neutralUnitId) const;


#pragma mark - ������
    /// �������� �������
    CommandPtr defend(const double x, const double y) const;

  };
}
