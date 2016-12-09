#include "T_Session.h"

#include "R_Standard.h"

using namespace AICup;


Session::Session() {
  role = std::make_shared<StandardRole>();
  skillBuild = std::make_shared<StandardSkillBuild>();
  strategyType = StrategyType::Base;
}

void Session::update(const model::Wizard& wizard, model::Move& move) {
  /// ���� ��������� �����:
  /// ���� ������� �� - ��������� ����������� ����������
  /// ���� ����������� �� - ��������� ���������
  /// ��������� ����

  role->update(wizard);
  skillBuild->update(wizard, move);
}