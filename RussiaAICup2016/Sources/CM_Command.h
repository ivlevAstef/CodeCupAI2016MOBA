//
//File: CM_Command.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#pragma once

#include <vector>
#include <memory>

namespace AICup
{
  class Command;
  class CommandFabric;
  typedef std::shared_ptr<Command> CommandPtr;

  class Command {
  public:
    /// ��������� �������, ���������� false ���� ������� ��������� ����������
    virtual bool execute() = 0;

    /// ������� ������� ����� ���������, ����� ����� ��������� ��������� ������� �������
    virtual const std::vector<CommandPtr>& neededCommands(const CommandFabric& fabric) = 0;
  };
}

