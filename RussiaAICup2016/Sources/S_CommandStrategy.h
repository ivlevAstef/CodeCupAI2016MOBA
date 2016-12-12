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

#include "R_Role.h"
#include "R_SkillBuild.h"

#ifdef ENABLE_VISUALIZATOR
#include "Visualizator.h"
#endif


namespace AICup
{
  class CommandStrategy {
  public:
    CommandStrategy(const CommandFabric& fabric, RolePtr role, SkillBuildPtr skillBuild);

    virtual void update(const model::Wizard& self, model::Move& move) = 0;

#ifdef ENABLE_VISUALIZATOR
    virtual void visualization(const model::Wizard& self, const Visualizator& visualizator) const;
#endif // ENABLE_VISUALIZATOR

  protected:
    void clear();

    void update(const Wizard& self, model::Move& move);
    Wizard preUpdate(const model::Wizard& self, model::Move& move);

  private:
    const Vector turn(const std::vector<MoveCommand::Result>& moveResults);
    bool move(std::vector<MoveCommand::Result>& moveResults, const Wizard& self, Vector& direction);

    const model::LivingUnit* attack(const Wizard& self, model::ActionType& action);
    const model::LivingUnit* cast(const Wizard& self, model::ActionType& action);

    std::vector<MoveCommand::Result> moveCommandsToMoveResult(const Wizard& self) const;

    /// просчитывает движение, на указанном векторе moveResults
    const Vector calcMoveVector(const std::vector<MoveCommand::Result>& moveResults, const Wizard& self);

    /// просчитывает столкновения, и добавляет деревья для уничтожения
    const Vector calculateCollisions(const Wizard& self, const Position& endPoint);

    void addTreeForRemove(const Wizard& self, const model::LivingUnit* tree);

    void addAvoidProjectiles(std::vector<MoveCommand::Result>& moveResults, const Wizard& self, const Vector& moveDirection);

  protected:
    const CommandFabric& fabric;
    std::shared_ptr<Algorithm::Path> path;


    RolePtr role;
    SkillBuildPtr skillBuild;

    std::vector<MoveCommandPtr> moveCommands;
    std::vector<AttackCommandPtr> attackCommands;
    std::vector<CastCommandPtr> castCommands;
  };

  typedef std::shared_ptr<CommandStrategy> CommandStrategyPtr;
};