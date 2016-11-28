#include "E_Wizard.h"
#include "R_StandardRole.h"
#include "C_Extensions.h"
#include "C_Math.h"

using namespace AICup;

Wizard::Wizard(const model::Wizard& wizard):
  model::Wizard(wizard.getId(),
    wizard.getX(), wizard.getY(), wizard.getSpeedX(), wizard.getSpeedY(), wizard.getAngle(),
    wizard.getFaction(), wizard.getRadius(), wizard.getLife(), wizard.getMaxLife(), wizard.getStatuses(),
    wizard.getOwnerPlayerId(), wizard.isMe(), wizard.getMana(), wizard.getMaxMana(),
    wizard.getVisionRange(), wizard.getCastRange(),
    wizard.getXp(), wizard.getLevel(), wizard.getSkills(),
    wizard.getRemainingActionCooldownTicks(), wizard.getRemainingCooldownTicksByAction(),
    wizard.isMaster(), wizard.getMessages())
{
  role = std::make_shared<StandardRole>();
}

void Wizard::update(model::Move& move) {
  role->update(*this, move);
}

void Wizard::moveInfoFrom(const Wizard& last) {
  this->role = last.role;
}
bool Wizard::availableAction(const model::ActionType action) const {
  return EX::availableSkills(*this)[action];
}

bool Wizard::isCooldown(const model::ActionType action) const {
  return cooldown(action) > 0;
}

int Wizard::cooldown(const model::ActionType action) const {
  if (!availableAction(action)) {
    return 9999999;
  }
  return MAX(this->getRemainingCooldownTicksByAction()[action], this->getRemainingActionCooldownTicks());
}

int Wizard::minAttackCooldown() const {
  const int cooldown1 = cooldown(model::ACTION_STAFF);
  const int cooldown2 = cooldown(model::ACTION_MAGIC_MISSILE);
  const int cooldown3 = cooldown(model::ACTION_FROST_BOLT);
  const int cooldown4 = cooldown(model::ACTION_FIREBALL);
  const int minCooldown = MIN(MIN(MIN(cooldown1, cooldown2), cooldown3), cooldown4);

  return minCooldown;
}

int Wizard::minStaffOrMissileCooldown() const {
  const int cooldown1 = cooldown(model::ACTION_STAFF);
  const int cooldown2 = cooldown(model::ACTION_MAGIC_MISSILE);
  const int minCooldown = MIN(cooldown1, cooldown2);

  return minCooldown;
}

double Wizard::maxSpeed() const {
  return EX::maxSpeed(*this);
}

double Wizard::maxBackwardSpeed() const {
  return EX::maxBackwardSpeed(*this);
}

double Wizard::maxTurnSpeed() const {
  return EX::turnSpeed(*this);
}

double Wizard::damage(const model::ActionType action) const {
  return EX::damage(*this, action);
}

double Wizard::dps(const model::ActionType action) const {
  return EX::dps(*this, action);
}