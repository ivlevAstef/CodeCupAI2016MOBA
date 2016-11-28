//
//File: CM_CommandStrategy.h
//Author: Ivlev Alexander. Stef
//Created: 13/11/2016
//

#pragma once

#include "E_Wizard.h"
#include "model\Move.h"

#include "CM_MoveCommand.h"
#include "CM_AttackCommand.h"
#include "CM_CommandFabric.h"

#ifdef ENABLE_VISUALIZATOR
#include "Visualizator.h"
#endif


namespace AICup
{
  class CommandStrategy {
  public:
    CommandStrategy(const CommandFabric& fabric, const Algorithm::PathFinder& pathFinder);

    virtual void update(const Wizard& self, model::Move& move);

#ifdef ENABLE_VISUALIZATOR
    virtual void visualization(const model::Wizard& self, const Visualizator& visualizator) const;
#endif // ENABLE_VISUALIZATOR

  protected:
    void clear();

  private:
    const Vector move(const std::vector<MoveCommand::Result>& moveResults, const Wizard& self, TurnStyle& turnStyle, Vector& turnDirection, double& speedLimit, bool& deactivateOtherTurn);
    const model::LivingUnit* attack(const Wizard& self, model::ActionType& action);

    std::vector<MoveCommand::Result> moveCommandsToMoveResult(const Wizard& self) const;

    void addTreeForRemove(const Wizard& self, const model::LivingUnit* tree);

  protected:
    const CommandFabric& fabric;
    const Algorithm::PathFinder& pathFinder;

    std::vector<MoveCommandPtr> moveCommands;
    std::vector<AttackCommandPtr> attackCommands;
  };

  typedef std::shared_ptr<CommandStrategy> CommandStrategyPtr;
};