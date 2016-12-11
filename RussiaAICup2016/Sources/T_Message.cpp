#include "T_Message.h"
#include <assert.h>
#include <cstring>

using namespace AICup;

Message::Message():
  Message(TacticsType::Unknown, model::_LANE_UNKNOWN_, 0, 0) {

}

Message::Message(TacticsType type):
  Message(type, model::_LANE_UNKNOWN_, 0, 0) {

}
Message::Message(TacticsType type, model::LaneType lane):
  Message(type, lane, 0, 0) {

}
Message::Message(TacticsType type, model::LaneType lane, int16_t attackTick):
  Message(type, lane, attackTick, 0) {

}
Message::Message(TacticsType type, int16_t attackTick, long long attackedWizardId):
  Message(type, model::_LANE_UNKNOWN_, attackTick, attackedWizardId) {
}

Message::Message(TacticsType type, model::LaneType lane, int16_t attackTick, long long attackedWizardId):
  tacticsType(type), lane(lane), attackTick(attackTick), attackedWizardId(attackedWizardId) {
}

struct MessageArchive {
  unsigned int tacticsType : 5; // 32 макс
  unsigned int attackTick : 15;
  unsigned int attackedWizardId : 4; /// воспользуемся тем фактом, что они всегда в первой 10, опасно это правда, но даже quickStart написан на этой основе
}; // summary: 24 = 32 - 4байта - 2 тика

const Message Message::read(const model::Message& message) {
  assert(4 == message.getRawMessage().size());

  const auto lane = message.getLane();

  const MessageArchive* archive = (const MessageArchive*)message.getRawMessage().data();
  /// тяжелые пошли времена, если memcpy на сервере не собирается
  //memcpy(&archive, message.getRawMessage().data(), sizeof(signed int) * 4);

  return Message((TacticsType)archive->tacticsType, (model::LaneType)lane, (int16_t)archive->attackTick, (long long)archive->attackedWizardId);
}

const model::Message Message::model() const {
  assert(0 <= int(tacticsType) && int(tacticsType) < 32);
  assert(0 <= attackTick && attackTick < 32768);
  assert(0 <= attackedWizardId && attackedWizardId < 16);

  MessageArchive archive;
  archive.tacticsType = (unsigned int)tacticsType;
  archive.attackTick = (unsigned int)attackTick;
  archive.attackedWizardId = (unsigned int)attackedWizardId;

  signed char buffer[4] = {0,0,0,0};
  memcpy(buffer, &archive, sizeof(signed char) * 4);

  std::vector<signed char> memory;
  memory.assign(buffer, buffer + 4);

  return model::Message(lane, model::_SKILL_UNKNOWN_, memory);
}