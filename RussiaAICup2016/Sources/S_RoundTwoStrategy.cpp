#include "S_RoundTwoStrategy.h"
#include "E_Game.h"
#include "E_World.h"

#include "R_RoundTwo.h"

using namespace AICup;

RoundTwoStrategy::RoundTwoStrategy(const CommandFabric& fabric) :
  BaseStrategyComponents(fabric, std::make_shared<RoundTwoRole>(), std::make_shared<RoundTwoSkillBuild>()) {
}

void RoundTwoStrategy::update(const model::Wizard& model, model::Move& move) {
  CommandStrategy::clear();
  const auto& self = CommandStrategy::preUpdate(model, move);

  if (100 <= World::model().getTickIndex() && World::model().getTickIndex() <= 500) {
    changeLane(self);
  } else {
    changeLane(self, 250);
  }

  ///////////////////////////////////

  addAroundEnemiesOrMoveMelee(self);
  addAttackFollow(self);


  ///////////////////////////////////

  addMoveTo(self, currentLane);

  const auto getExpirienceCommand = fabric.moveGetExpirience();
  if (getExpirienceCommand->check(self)) {
    moveCommands.push_back(getExpirienceCommand);
  }

  ///////////////////////////////////////

  addAttacks(self);
  addCasts(self);

  CommandStrategy::update(self, move);
}

//// ��� ��������� ����
void RoundTwoStrategy::dynamicChangeSkillBuild(const model::Wizard& self) {
  /// ���� ��� ��� ������ ����� �� �����, �� � �� ������
  if (self.getLevel() > 0) {
    return;
  }

  const auto myLine = World::instance().positionToLine(self.getX(), self.getY());

  /// ���� ����� �� ��������, �������� ��� ���� � ���, ��� �������� ������
  if (myLine == model::_LANE_UNKNOWN_) {
    skillBuild = std::make_shared<RoundTwoSkillBuild>();
    role = std::make_shared<RoundTwoRole>();
    return;
  }

  const auto friendWizard = World::instance().wizardCount(myLine, Game::friendFaction());
  const auto enemyWizard = World::instance().wizardCount(myLine, Game::enemyFaction());
  const auto wizardImbalance = friendWizard - enemyWizard;


  if (wizardImbalance == 0) { /// ���� ���� ����� �� ��������� �������
    skillBuild = std::make_shared<RoundTwoSkillBuild>();
    role = std::make_shared<RoundTwoRole>();

  } else if (wizardImbalance >= 1) { /// ����� ������
    skillBuild = std::make_shared<FastPushSkillBuild>();
    role = std::make_shared<FastPushRole>();

  } else if (wizardImbalance <= -1) { /// ����� ������
    skillBuild = std::make_shared<HardLineSkillBuild>();
    role = std::make_shared<HardLineRole>();

  }
}