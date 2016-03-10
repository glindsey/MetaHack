#ifndef GENDER_H
#define GENDER_H

#include "stdafx.h"

#include "LuaObject.h"

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

inline std::ostream& operator<<(std::ostream& os, Gender gender)
{
  switch (gender)
  {
    case Gender::None: os << "None"; break;
    case Gender::Male: os << "Male"; break;
    case Gender::Female: os << "Female"; break;
    case Gender::Neuter: os << "Neuter"; break;
    case Gender::Spivak: os << "Spivak"; break;
    case Gender::FirstPerson: os << "FirstPerson"; break;
    case Gender::SecondPerson: os << "SecondPerson"; break;
    case Gender::Plural: os << "Plural"; break;
    case Gender::Hanar: os << "Hanar"; break;
    case Gender::UnknownThing: os << "UnknownThing"; break;
    case Gender::UnknownPerson: os << "UnknownPerson"; break;
    case Gender::Count: os << "Count"; break;
    default: os << "???"; break;
  }

  return os;
}

inline void Gender_add_to_lua(Lua* lua_instance)
{
  lua_instance->add_enum("Gender",
                         "None", Gender::None,
                         "Male", Gender::Male,
                         "Female", Gender::Female,
                         "Neuter", Gender::Neuter,
                         "Spivak", Gender::Spivak,
                         "FirstPerson", Gender::FirstPerson,
                         "SecondPerson", Gender::SecondPerson,
                         "Plural", Gender::Plural,
                         "Hanar", Gender::Hanar,
                         "UnknownThing", Gender::UnknownThing,
                         "UnknownPerson", Gender::UnknownPerson,
                         "Count", Gender::Count,
                         0
                         );
}

StringDisplay const& getSubjPro(Gender gender);
StringDisplay const& getObjPro(Gender gender);
StringDisplay const& getRefPro(Gender gender);
StringDisplay const& getPossAdj(Gender gender);
StringDisplay const& getPossPro(Gender gender);
StringDisplay const& getIndefArt(StringDisplay const& desc);

#endif // GENDER_H
