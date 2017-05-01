#include "components/ComponentBodyparts.h"

#include "game/GameState.h"
#include "utilities/JSONUtils.h"

void from_json(json const& j, ComponentBodyparts& obj)
{
  obj = ComponentBodyparts();

  // *** add Component-specific assignments here ***
  JSONUtils::doIfPresent(j,   "skin", [&](auto& value) { obj.populateUsingJSON(BodyPart::Skin, value); });
  JSONUtils::doIfPresent(j,   "head", [&](auto& value) { obj.populateUsingJSON(BodyPart::Head, value); });
  JSONUtils::doIfPresent(j,    "ear", [&](auto& value) { obj.populateUsingJSON(BodyPart::Ear, value); });
  JSONUtils::doIfPresent(j,    "eye", [&](auto& value) { obj.populateUsingJSON(BodyPart::Eye, value); });
  JSONUtils::doIfPresent(j,   "nose", [&](auto& value) { obj.populateUsingJSON(BodyPart::Nose, value); });
  JSONUtils::doIfPresent(j,   "neck", [&](auto& value) { obj.populateUsingJSON(BodyPart::Neck, value); });
  JSONUtils::doIfPresent(j,  "chest", [&](auto& value) { obj.populateUsingJSON(BodyPart::Chest, value); });
  JSONUtils::doIfPresent(j,    "arm", [&](auto& value) { obj.populateUsingJSON(BodyPart::Arm, value); });
  JSONUtils::doIfPresent(j,   "hand", [&](auto& value) { obj.populateUsingJSON(BodyPart::Hand, value); });
  JSONUtils::doIfPresent(j, "finger", [&](auto& value) { obj.populateUsingJSON(BodyPart::Finger, value); });
  JSONUtils::doIfPresent(j,  "torso", [&](auto& value) { obj.populateUsingJSON(BodyPart::Torso, value); });
  JSONUtils::doIfPresent(j,    "leg", [&](auto& value) { obj.populateUsingJSON(BodyPart::Leg, value); });
  JSONUtils::doIfPresent(j,   "foot", [&](auto& value) { obj.populateUsingJSON(BodyPart::Foot, value); });
  JSONUtils::doIfPresent(j,    "toe", [&](auto& value) { obj.populateUsingJSON(BodyPart::Toe, value); });
  JSONUtils::doIfPresent(j,   "wing", [&](auto& value) { obj.populateUsingJSON(BodyPart::Wing, value); });
  JSONUtils::doIfPresent(j,   "tail", [&](auto& value) { obj.populateUsingJSON(BodyPart::Tail, value); });
  JSONUtils::doIfPresent(j,   "prehensile-tail", [&](auto& value) { obj.populateUsingJSON(BodyPart::PTail, value); });
}

void to_json(json& j, ComponentBodyparts const& obj)
{
  j = json::object();
  j["skin"] = obj.bodyPartToJSON(BodyPart::Skin);
  j["head"] = obj.bodyPartToJSON(BodyPart::Head);
  j["ear"] = obj.bodyPartToJSON(BodyPart::Ear);
  j["eye"] = obj.bodyPartToJSON(BodyPart::Eye);
  j["nose"] = obj.bodyPartToJSON(BodyPart::Nose);
  j["neck"] = obj.bodyPartToJSON(BodyPart::Neck);
  j["chest"] = obj.bodyPartToJSON(BodyPart::Chest);
  j["arm"] = obj.bodyPartToJSON(BodyPart::Arm);
  j["hand"] = obj.bodyPartToJSON(BodyPart::Hand);
  j["finger"] = obj.bodyPartToJSON(BodyPart::Finger);
  j["torso"] = obj.bodyPartToJSON(BodyPart::Torso);
  j["leg"] = obj.bodyPartToJSON(BodyPart::Leg);
  j["foot"] = obj.bodyPartToJSON(BodyPart::Foot);
  j["toe"] = obj.bodyPartToJSON(BodyPart::Toe);
  j["wing"] = obj.bodyPartToJSON(BodyPart::Wing);
  j["tail"] = obj.bodyPartToJSON(BodyPart::Tail);
  j["prehensile-tail"] = obj.bodyPartToJSON(BodyPart::PTail);  // *** add Component-specific assignments here ***
  //j["member"] = obj.m_member;
}

json ComponentBodyparts::bodyPartToJSON(BodyPart part) const
{
  json result = json::object();
  result["count"] = typicalCount(part);
  result["existence"] = json::array();
  auto& existence = result["existence"];
  for (unsigned int counter = 0; counter < typicalCount(part); ++counter)
  {
    existence.push_back(exists(part, counter));
  }

  return result;
}

void ComponentBodyparts::populateUsingJSON(BodyPart part, json const& j)
{
  if (!j.is_object()) return;
  if (j.count("count") == 0) return;
  unsigned int count = j["count"].get<unsigned int>();
  setUpBodyparts(part, count);

  if (j.count("existence") == 0) return;
  auto& existence = j["existence"];
  if (!existence.is_array() || existence.size() != count) return;

  for (unsigned int counter = 0; counter < count; ++counter)
  {
    m_bodypartExistence[static_cast<unsigned int>(part)][counter] = existence[counter].get<bool>();
  }
}

unsigned int ComponentBodyparts::typicalCount(BodyPart part) const
{
  return m_bodypartCount[static_cast<unsigned int>(part)];
}

size_t ComponentBodyparts::actualCount(BodyPart part) const
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

bool ComponentBodyparts::hasPrehensileBodyPart() const
{
  return (actualCount(BodyPart::Finger) > 0) || (actualCount(BodyPart::PTail) > 0);
}

void ComponentBodyparts::setUpBodyparts(BodyPart part, unsigned int count)
{
  m_bodypartCount[static_cast<unsigned int>(part)] = count;
  m_bodypartExistence[static_cast<unsigned int>(part)].resize(count);
  m_bodypartExistence[static_cast<unsigned int>(part)].set();
}
