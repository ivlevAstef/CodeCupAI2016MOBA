//
//File: CM_Command.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#pragma once

#include <memory>
#include "model\Wizard.h"

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
    virtual bool check(const model::Wizard& self) = 0;


#ifdef ENABLE_VISUALIZATOR
    virtual void visualization(const Visualizator&) const { }
#endif // ENABLE_VISUALIZATOR
  };
}

