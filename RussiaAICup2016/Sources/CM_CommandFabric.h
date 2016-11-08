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

#pragma mark - �����������
    /// ������������� � �����
    CommandPtr moveToPoint(double x, double y);

    /// ������������� �� �����
    CommandPtr moveToLine(model::LineType line);

    /// ��������� �� ������
    CommandPtr follow(long long unitId);

    /// ������������ ����� - ��������� ���� ��� ��� �����
    CommandPtr observeMap();

#pragma mark - �����
    /// ��������� �����
    CommandPtr attack(long long enemyId);

    /// ��� ����������� ������
    CommandPtr pool(long long neutralUnitId);


#pragma mark - ������
    /// �������� �������
    CommandPtr defend(double x, double y);

  };
}
