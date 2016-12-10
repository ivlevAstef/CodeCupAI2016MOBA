#include "T_Session.h"

#include "E_World.h"
#include "E_Game.h"

#include "R_Standard.h"
#include "R_DynamicSkillBuilds.h"

using namespace AICup;


Session::Session() {
  role = std::make_shared<StandardRole>();
  skillBuild = std::make_shared<StandardSkillBuild>();
  strategyType = StrategyType::Base;
}

void Session::update(const model::Wizard& self, model::Move& move) {
  /// ���� ��������� �����
  if (Game::model().isRawMessagesEnabled()) {
    /// ���� ������� �� - ��������� ����������� ����������
    /// ���� ����������� �� - ��������� ���������
    /// ��������� ����
  } else {
    dynamicChangeSkillBuild(self);
  }

  role->update(self);
  skillBuild->update(self, move);
}

void Session::dynamicChangeSkillBuild(const model::Wizard& self) {
  /// ���� ��� ��� ������ ����� �� �����, �� � �� ������
  if (self.getLevel() > 0) {
    return;
  }

  const auto myLine = World::instance().positionToLine(self.getX(), self.getY());

  /// ���� ����� �� ��������, �������� ��� ���� � ���, ��� �������� ������
  if (myLine == model::_LANE_UNKNOWN_) {
    skillBuild = std::make_shared<StandardSkillBuild>();
    role = std::make_shared<StandardRole>();
    return;
  }

  const auto friendWizard = World::instance().wizardCount(myLine, Game::friendFaction());
  const auto enemyWizard = World::instance().wizardCount(myLine, Game::enemyFaction());
  const auto wizardImbalance = friendWizard - enemyWizard;


  if (wizardImbalance == 0) { /// ���� ���� ����� �� ��������� �������
    skillBuild = std::make_shared<StandardSkillBuild>();
    role = std::make_shared<StandardRole>();

  } else if (wizardImbalance >= 1) { /// ����� ������
    skillBuild = std::make_shared<FastPushSkillBuild>();
    role = std::make_shared<FastPushRole>();

  } else if (wizardImbalance <= -1) { /// ����� ������
    skillBuild = std::make_shared<HardLineSkillBuild>();
    role = std::make_shared<HardLineRole>();

  }
}