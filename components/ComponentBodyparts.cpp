#include "components/ComponentBodyparts.h"

#include "game/GameState.h"
#include "utilities/JSONUtils.h"

void from_json(json const& j, ComponentBodyparts& obj)
{
  obj = ComponentBodyparts();

  // *** add Component-specific assignments here ***
  JSONUtils::doIfPresent(j,  "skin-count", [&](auto& value) { obj.setUpBodyparts(BodyPart::Skin, value); });
  JSONUtils::doIfPresent(j,  "head-count", [&](auto& value) { obj.setUpBodyparts(BodyPart::Head, value); });
  JSONUtils::doIfPresent(j,   "ear-count", [&](auto& value) { obj.setUpBodyparts(BodyPart::Ear, value); });
  JSONUtils::doIfPresent(j,   "eye-count", [&](auto& value) { obj.setUpBodyparts(BodyPart::Eye, value); });
  JSONUtils::doIfPresent(j,  "nose-count", [&](auto& value) { obj.setUpBodyparts(BodyPart::Nose, value); });
  JSONUtils::doIfPresent(j,  "neck-count", [&](auto& value) { obj.setUpBodyparts(BodyPart::Neck, value); });
  JSONUtils::doIfPresent(j, "chest-count", [&](auto& value) { obj.setUpBodyparts(BodyPart::Chest, value); });
  JSONUtils::doIfPresent(j,   "arm-count", [&](auto& value) { obj.setUpBodyparts(BodyPart::Arm, value); });
  JSONUtils::doIfPresent(j,  "hand-count", [&](auto& value) { obj.setUpBodyparts(BodyPart::Hand, value); });
  JSONUtils::doIfPresent(j, "torso-count", [&](auto& value) { obj.setUpBodyparts(BodyPart::Torso, value); });
  JSONUtils::doIfPresent(j,   "leg-count", [&](auto& value) { obj.setUpBodyparts(BodyPart::Leg, value); });
  JSONUtils::doIfPresent(j,  "foot-count", [&](auto& value) { obj.setUpBodyparts(BodyPart::Foot, value); });
  JSONUtils::doIfPresent(j,  "wing-count", [&](auto& value) { obj.setUpBodyparts(BodyPart::Wing, value); });
  JSONUtils::doIfPresent(j,  "tail-count", [&](auto& value) { obj.setUpBodyparts(BodyPart::Tail, value); });
  JSONUtils::doIfPresent(j,  "prehensile-tail-count", [&](auto& value) { obj.setUpBodyparts(BodyPart::PTail, value); });
}

void to_json(json& j, ComponentBodyparts const& obj)
{
  j = json::object();
  // *** add Component-specific assignments here ***
  //j["member"] = obj.m_member;
}

unsigned int ComponentBodyparts::typicalCount(BodyPart part)
{
  return m_bodypartCount[static_cast<unsigned int>(part)];
}

size_t ComponentBodyparts::actualCount(BodyPart part)
{
  return m_bodypartExistence[static_cast<unsigned int>(part)].count();
}

boost::dynamic_bitset<size_t>::reference ComponentBodyparts::exists(BodyPart part, unsigned int which)
{
  return m_bodypartExistence[static_cast<unsigned int>(part)][which];
}

bool ComponentBodyparts::exists(BodyPart part, unsigned int which) const
{
  return m_bodypartExistence[static_cast<unsigned int>(part)][which];
}

void ComponentBodyparts::setUpBodyparts(BodyPart part, unsigned int count)
{
  m_bodypartCount[static_cast<unsigned int>(part)] = count;
  m_bodypartExistence[static_cast<unsigned int>(part)].resize(count);
  m_bodypartExistence[static_cast<unsigned int>(part)].set();
}
