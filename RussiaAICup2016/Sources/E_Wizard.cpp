#include "E_Wizard.h"
#include "R_StandardRole.h"


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