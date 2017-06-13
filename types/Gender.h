#pragma once
/// @file Gender.h Handles gender-specific language.
/// @todo Make this localizable. (That's going to be a MAJOR pain.)

#include <ostream>
#include <string>

#include "json.hpp"
using json = ::nlohmann::json;

/// Enumeration of not just genders, but also other modes of speech that might
/// be required.
/// @note If this class is changed, remember to also change the method that
///       adds it to Lua in LuaObject.h!
enum class Gender
{
  None = 0,
  Male,
  Female,
  Neuter,
  Spivak,
  FirstPerson,
  SecondPerson,
  Plural,
  Hanar,
  UnknownEntity,
  UnknownPerson,
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
    case Gender::UnknownEntity: os << "UnknownEntity"; break;
    case Gender::UnknownPerson: os << "UnknownPerson"; break;
    case Gender::Count: os << "Count"; break;
    default: os << "???"; break;
  }

  return os;
}

void from_json(json const& j, Gender& obj);
void to_json(json& j, Gender const& obj);

std::string const& getSubjPro(Gender gender);
std::string const& getObjPro(Gender gender);
std::string const& getRefPro(Gender gender);
std::string const& getPossAdj(Gender gender);
std::string const& getPossPro(Gender gender);
std::string const& getIndefArt(std::string const& desc);
std::string const& getDefArt(std::string const& desc);