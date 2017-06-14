#include "stdafx.h"

#include "types/BodyPart.h"

/// Map of bodypart names to enum values.
/// @warning This map should contain an entry for each enum value, or there
///          will be JSON serialization/deserialization problems!
BodyPartBimap const s_bodyPartBimap = boost::assign::list_of<BodyPartBimap::relation>
("nowhere", BodyPart::Nowhere)
("unknown", BodyPart::Unknown)
("skin", BodyPart::Skin)
("head", BodyPart::Head)
("face", BodyPart::Face)
("ear", BodyPart::Ear)
("eye", BodyPart::Eye)
("nose", BodyPart::Nose)
("mouth", BodyPart::Mouth)
("neck", BodyPart::Neck)
("chest", BodyPart::Chest)
("arm", BodyPart::Arm)
("hand", BodyPart::Hand)
("finger", BodyPart::Finger)
("torso", BodyPart::Torso)
("leg", BodyPart::Leg)
("foot", BodyPart::Foot)
("toe", BodyPart::Toe)
("wing", BodyPart::Wing)
("tail", BodyPart::Tail)
("ptail", BodyPart::PTail);

void from_json(json const& j, BodyPart& obj)
{
  auto& strsToParts = BodyParts::strsToParts();

  obj = BodyPart::Nowhere;
  if (j.is_string())
  {
    std::string str = boost::to_lower_copy(j.get<std::string>());
    boost::trim(str);

    if (strsToParts.count(str) != 0)
    {
      obj = strsToParts.at(str);
    }
  }
}

void to_json(json& j, BodyPart const& obj)
{
  auto& partsToStrs = BodyParts::partsToStrs();

  j = "unknown";

  if (partsToStrs.count(obj) != 0)
  {
    j = partsToStrs.at(obj);
  }
}

std::ostream& operator<<(std::ostream& os, BodyPart const& part)
{
  auto& partsToStrs = BodyParts::partsToStrs();

  if (partsToStrs.count(part) != 0)
  {
    os << partsToStrs.at(part);
  }
  else
  {
    os << "unknown";
  }

  return os;
}

BodyPartBimap::left_map const & BodyParts::strsToParts()
{
  return s_bodyPartBimap.left;
}

BodyPartBimap::right_map const & BodyParts::partsToStrs()
{
  return s_bodyPartBimap.right;
}
