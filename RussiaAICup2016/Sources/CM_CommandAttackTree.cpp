#include "CM_CommandAttackTree.h"
#include "E_World.h"
#include "E_Game.h"
#include "C_Math.h"
#include "C_Extensions.h"
#include "CM_AttackPriorities.h"
#include "A_Attack.h"

using namespace AICup;

CommandAttackTree::CommandAttackTree(const model::Tree& tree) : tree(tree) {
}

bool CommandAttackTree::check(const Wizard& self) {
  const auto selfPos = EX::pos(self);
  const auto treePos = EX::pos(tree);
  const auto delta = selfPos - treePos;

  /// дерево далеко
  if (delta.length() > self.getCastRange() + tree.getRadius()) {
    return false;
  }


  /// Если еще много времени до кд, то не стоит атаковать
  if (self.minStaffOrMissileCooldown() > Algorithm::timeToTurnForAttack(tree, self) + 1) {
    return false;
  }

  /// если на пути другое дерево, то не стреляем
  if (Algorithm::checkIntersectedTree(selfPos, treePos, Game::model().getMagicMissileRadius(), tree.getId())) {
    return false;
  }

  /// во всех остальных случаях возможна или атака, или поворот к дереву для атаки
  return true;
}


void CommandAttackTree::execute(const Wizard& self, Result& result) {
  const double distance = self.getDistanceTo(tree);

  result.unit = &tree;
  result.priority = self.getRole().getTreePriority() * AttackPriorities::attackTree(self, tree);

  if (Algorithm::isMelee(self, tree) && !self.isCooldown(model::ACTION_STAFF)) {
    result.action = model::ACTION_STAFF;
  } else {
    result.action = model::ACTION_MAGIC_MISSILE;
  }
}

#ifdef ENABLE_VISUALIZATOR
void CommandAttackTree::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  if (Visualizator::PRE == visualizator.getStyle()) {
    visualizator.line(self.getX(), self.getY(), tree.getX(), tree.getY(), 0xffaa00);
  }

  if (Visualizator::POST == visualizator.getStyle()) {
    visualizator.fillCircle(tree.getX(), tree.getY(), tree.getRadius() * 0.75, 0xffaa00);
  }
}
#endif // ENABLE_VISUALIZATOR