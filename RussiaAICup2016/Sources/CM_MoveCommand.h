//
//File: CM_MoveCommand.h
//Author: Ivlev Alexander. Stef
//Created: 17/11/2016
//

#pragma once

#include "CM_Command.h"
#include "E_Types.h"
#include "C_Vector2D.h"
#include "A_PathFinder.h"

namespace AICup
{
  class MoveCommand: public Command {
  public:
    struct Result {
      Vector moveDirection; /// ������ � ������
      Vector turnDirection;

      double priority; /// ������ ��� ��� ������ �������, ��� ��������� ����� �������, �� ��� ���� ��� �� ��������������!
      double turnPriority;

      void set(const Position& position, const Wizard& self);
    };

  public:
    /// ��������� �������
    virtual void execute(const Wizard& self, Result& result) = 0;
  };

  typedef std::shared_ptr<MoveCommand> MoveCommandPtr;
}

