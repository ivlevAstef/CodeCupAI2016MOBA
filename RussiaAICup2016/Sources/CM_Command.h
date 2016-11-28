//
//File: CM_Command.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#pragma once

#include <memory>
#include "E_Wizard.h"

#ifdef ENABLE_VISUALIZATOR
#include "Visualizator.h"
#endif

namespace AICup
{
  class Command;
  typedef std::shared_ptr<Command> CommandPtr;

  class Command {
  public:

    /// ��������� ����������� ���������� �������
    virtual bool check(const Wizard&) = 0;

    /// �������� �� 1000, ���� ��� ������ 1 �� ������� ������������
    virtual double priority(const Wizard& self) = 0;

#ifdef ENABLE_VISUALIZATOR
    virtual void visualization(const model::Wizard&, const Visualizator&) const { }
#endif // ENABLE_VISUALIZATOR
  };
}

