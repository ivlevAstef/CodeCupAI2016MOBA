//
//File: CM_CommandFabric.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#pragma once

#include "CM_Command.h"
#include "model\LineType.h"
#include <initializer_list>

namespace AICup
{
  class CommandFabric {
  public:
#pragma mark - �����������
    /// ������������� � �����
    CommandPtr moveToPoint(double x, double y) const;

    /// ������������� �� �����
    CommandPtr moveToLine(model::LineType line) const;

    /// ��������� �� ������
    CommandPtr follow(long long unitId) const;

    /// ������������ ����� - ��������� ���� ��� ��� �����
    CommandPtr observeMap() const;

#pragma mark - �����
    /// ��������� �����
    CommandPtr attack(long long enemyId) const;

    /// ��� ����������� ������
    CommandPtr pool(long long neutralUnitId) const;


#pragma mark - ������
    /// �������� �������
    CommandPtr defend(double x, double y) const;

  };
}
