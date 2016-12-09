#include "T_Session.h"

#include "R_Standard.h"

using namespace AICup;


Session::Session() {
  role = std::make_shared<StandardRole>();
  skillBuild = std::make_shared<StandardSkillBuild>();
  strategyType = StrategyType::Base;
}

void Session::update(const model::Wizard& wizard, model::Move& move) {
  /// если командный режим:
  /// если главный то - запускаем тактический анализатор
  /// если подчиненный то - считываем сообщения
  /// обновляем инфу

  role->update(wizard);
  skillBuild->update(wizard, move);
}