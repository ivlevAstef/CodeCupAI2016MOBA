#include "R_Role.h"
#include "E_World.h"

using namespace AICup;

Role::Role() {
  useStartedLinePriority = true;

  audacity = 1;
  linePressureWizards = 1;
  lineAudacityWizards = 1;
  importanceOfXP = 1;
  importanceOfBonus = 1;

  buildPriority = 1;
  minionPriority = 1;
  treePriority = 1;
  wizardPriority = 1;
  wizardByIdPriority = 0;
  attackSkillPriority = 1;

  audacityBuild = 1;
  audacityMinion = 1;
  audacityWizard = 1;

  repulsionDodgeFireballPriority = 0;

  desireChangeLine = 1;
  changeLineForeFrontPriority = 1;
  changeLinePathLengthPriority = 1;
  changeLineWizardCountPriority = 1;
  changeLineWizardCountOnlyFriend = false;
  changeLineTowerBalancePriority = 1;
  changeLineLaneStrengthPriority = 1;
  changeLineExpiriencePriority = 0.5;

  attackMeleeWinThreshold = 0.5;
  attackWizardMeleePriority = 1;
  attackMinionMeleePriority = 1;
  attackBuildMeleePriority = 1;

  friendWizardConfidence = 0.5;
}


void Role::update(const model::Wizard& self) {
  useStartedLinePriority = (World::model().getTickIndex() < 500);
}