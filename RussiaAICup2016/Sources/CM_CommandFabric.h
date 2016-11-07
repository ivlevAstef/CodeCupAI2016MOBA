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
    /// ������������� � �����
    CommandPtr moveToPoint(double x, double y);

    /// ������������� �� �����
    CommandPtr moveToLine(model::LineType line);

    /// ��������� �����
    CommandPtr attack(long long enemyId);

    /// �������� �������
    CommandPtr defend(double x, double y);

    /// ��� ����������� ������
    //CommandPtr pool(double x, double y);

    /// ��� ����������� �������������� ���������� ������
    CommandPtr sequence(std::initializer_list<CommandPtr> commands);
  };
}
