//
//File: T_Message.h
//Author: Ivlev Alexander. Stef
//Created: 10/12/2016
//


#include "model\Message.h"
#include "model\LaneType.h"
#include "T_TacticsType.h"
#include <memory>

#pragma once

namespace AICup {
  /// ��������� � ��������� ������ �� ���������, � ������������ ���������
  class Message {
  public:
    Message(); /// ������

    // ����� ���� �������� � union, �� ���� �����
    // Standard: lane
    // AntiRush: lane, attackTick
    // Push: lane
    // Attack: attackedWizardId, attackTick
    // Defense: lane
    Message(TacticsType type);
    Message(TacticsType type, model::LaneType lane);
    Message(TacticsType type, model::LaneType lane, int16_t attackTick);
    Message(TacticsType type, int16_t attackTick, long long attackedWizardId);

    static const Message read(const model::Message& message);

    const model::Message model() const;

    inline bool getIsEmpty() const {
      return TacticsType::Unknown == tacticsType;
    }

    inline TacticsType getTacticsType() const {
      return tacticsType;
    }

    inline model::LaneType getLane() const {
      return lane;
    }

    inline int getAttackTick() const {
      return int(attackTick);
    }

    inline long long getAttackedWizardId() const {
      return attackedWizardId;
    }

  private:
    Message(TacticsType type, model::LaneType lane, int16_t attackTick, long long attackedWizardId);

  private:
    const TacticsType tacticsType;

    /// ������, �� ����������� ������������ ���
    const model::LaneType lane;
    const int16_t attackTick;
    const long long attackedWizardId;
  };
};