//
//File: CM_CommandPool.h
//Author: Ivlev Alexander. Stef
//Created: 08/11/2016
//


#pragma once

#include "CM_AttackCommand.h"

namespace AICup
{
  class CommandPool: public AttackCommand {
  public:
    CommandPool(long long neutralUnitId);

    bool check(const model::Wizard& self) override;

    int priority(const model::Wizard& self) override;

    void execute(const model::Wizard& self, Result& result) override;

  private:
  };
}