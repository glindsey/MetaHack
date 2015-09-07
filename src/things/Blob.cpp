#include "things/Blob.h"

Blob::Blob(std::string type, UUID id) : Thing(type, id) {}
Blob::~Blob() {}

unsigned int Blob::get_bodypart_number(BodyPart part) const
{
  switch (part)
  {
    case BodyPart::Body:  return 1;
    case BodyPart::Skin:  return 0;
    case BodyPart::Head:  return 0;
    case BodyPart::Ear:   return 0;
    case BodyPart::Eye:   return 0;
    case BodyPart::Nose:  return 0;
    case BodyPart::Mouth: return 0;
    case BodyPart::Neck:  return 0;
    case BodyPart::Chest: return 0;
    case BodyPart::Arm:   return 0;
    case BodyPart::Hand:  return 0;
    case BodyPart::Leg:   return 0;
    case BodyPart::Foot:  return 0;
    default:
      return 0;
  }
}

std::string Blob::get_bodypart_name(BodyPart part) const
{
  switch (part)
  {
    case BodyPart::Body:  return "mass";
    default:
      return "invalid body part";
  }
}

std::string Blob::get_bodypart_plural(BodyPart part) const
{
  switch (part)
  {
    case BodyPart::Body:  return "masses";
    default:
      return "invalid body part";
  }
}

bool Blob::can_currently_see() const
{
  return false;
}

bool Blob::can_currently_move() const
{
  return false;
}

