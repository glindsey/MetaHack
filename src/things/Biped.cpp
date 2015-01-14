#include "things/Biped.h"

Biped::Biped() : Entity() {}
Biped::~Biped() {}

sf::Vector2u Biped::get_tile_sheet_coords(int frame) const
{
  if (get_attributes().get(Attribute::HP) > 0)
  {
    return sf::Vector2u(0, 4);
  }
  else
  {
    return sf::Vector2u(1, 4);
  }
}

unsigned int Biped::get_bodypart_number(BodyPart part) const
{
  switch (part)
  {
    case BodyPart::Body:  return 1;
    case BodyPart::Skin:  return 1;
    case BodyPart::Head:  return 1;
    case BodyPart::Ear:   return 2;
    case BodyPart::Eye:   return 2;
    case BodyPart::Nose:  return 1;
    case BodyPart::Mouth: return 1;
    case BodyPart::Neck:  return 1;
    case BodyPart::Chest: return 1;
    case BodyPart::Arm:   return 2;
    case BodyPart::Hand:  return 2;
    case BodyPart::Leg:   return 2;
    case BodyPart::Foot:  return 2;
    default:
      return 0;
  }
}

std::string Biped::get_bodypart_name(BodyPart part) const
{
  switch (part)
  {
    case BodyPart::Body:  return "body";
    case BodyPart::Skin:  return "skin";
    case BodyPart::Head:  return "head";
    case BodyPart::Ear:   return "ear";
    case BodyPart::Eye:   return "eye";
    case BodyPart::Nose:  return "nose";
    case BodyPart::Mouth: return "mouth";
    case BodyPart::Neck:  return "neck";
    case BodyPart::Chest: return "chest";
    case BodyPart::Arm:   return "arm";
    case BodyPart::Hand:  return "hand";
    case BodyPart::Leg:   return "leg";
    case BodyPart::Foot:  return "foot";
    default:
      return "invalid body part";
  }
}

std::string Biped::get_bodypart_plural(BodyPart part) const
{
  switch (part)
  {
    case BodyPart::Body:  return "bodies";
    case BodyPart::Skin:  return "skins";
    case BodyPart::Head:  return "heads";
    case BodyPart::Ear:   return "ears";
    case BodyPart::Eye:   return "eyes";
    case BodyPart::Nose:  return "noses";
    case BodyPart::Mouth: return "mouths";
    case BodyPart::Neck:  return "necks";
    case BodyPart::Chest: return "chests";
    case BodyPart::Arm:   return "arms";
    case BodyPart::Hand:  return "hands";
    case BodyPart::Leg:   return "legs";
    case BodyPart::Foot:  return "feet";
    default:
      return "invalid body part";
  }
}
