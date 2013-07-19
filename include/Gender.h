#ifndef GENDER_H
#define GENDER_H

#include <string>

/// Enumeration of not just genders, but also other modes of speech that might
/// be required.
enum class Gender
{
  None = 0,
  Male = 1,
  Female = 2,
  Neuter = 3,
  Spivak = 4,
  FirstPerson = 5,
  SecondPerson = 6,
  Plural = 7,
  Hanar = 8,
  UnknownThing = 9,
  UnknownPerson = 10,
  Count
};

std::string const& getSubjPro(Gender gender);
std::string const& getObjPro(Gender gender);
std::string const& getRefPro(Gender gender);
std::string const& getPossAdj(Gender gender);
std::string const& getPossPro(Gender gender);
std::string const& getIndefArt(std::string const& desc);

#endif // GENDER_H
