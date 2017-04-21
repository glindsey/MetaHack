#include "stdafx.h"

#include "types/Gender.h"

std::string const subjectPronounArray[] =
{
  "it",
  "he",
  "she",
  "ze",
  "ey",
  "I",
  "you",
  "they",
  "this one",
  "something",
  "someone"
};

std::string const objectPronounArray[] =
{
  "it",
  "him",
  "her",
  "hir",
  "em",
  "me",
  "you",
  "them",
  "this one",
  "something",
  "someone"
};

std::string const reflexivePronounArray[] =
{
  "itself",
  "himself",
  "herself",
  "hirself",
  "emself",
  "myself",
  "yourself",
  "themselves",
  "itself",
  "itself",
  "him/herself"
};

std::string const possessiveAdjectiveArray[] =
{
  "its",
  "his",
  "her",
  "hir",
  "eir",
  "my",
  "your",
  "their",
  "its",
  "its",
  "his/her"
};

std::string const possessivePronounArray[] =
{
  "its",
  "his",
  "hers",
  "hirs",
  "eirs",
  "mine",
  "yours",
  "theirs",
  "this one's",
  "its",
  "his/hers"
};

void from_json(json const& j, Gender& obj)
{
  obj = static_cast<Gender>(j.get<unsigned int>());
}

void to_json(json& j, Gender const& obj)
{
  j = static_cast<unsigned int>(obj);
}

std::string const& getSubjPro(Gender gender)
{
  if ((int)gender < (int)Gender::Count)
  {
    return subjectPronounArray[(int)gender];
  }
  else
  {
    return subjectPronounArray[0];
  }
}

std::string const& getObjPro(Gender gender)
{
  if ((int)gender < (int)Gender::Count)
  {
    return objectPronounArray[(int)gender];
  }
  else
  {
    return objectPronounArray[0];
  }
}

std::string const& getRefPro(Gender gender)
{
  if ((int)gender < (int)Gender::Count)
  {
    return reflexivePronounArray[(int)gender];
  }
  else
  {
    return reflexivePronounArray[0];
  }
}

std::string const& getPossAdj(Gender gender)
{
  if ((int)gender < (int)Gender::Count)
  {
    return possessiveAdjectiveArray[(int)gender];
  }
  else
  {
    return possessiveAdjectiveArray[0];
  }
}

std::string const& getPossPro(Gender gender)
{
  if ((int)gender < (int)Gender::Count)
  {
    return possessivePronounArray[(int)gender];
  }
  else
  {
    return possessivePronounArray[0];
  }
}

std::string const& getIndefArt(std::string const& desc)
{
  static std::string const articleAn = "an";
  static std::string const articleA = "a";
  static std::string const articleNull = "";

  if (desc.empty())
  {
    return articleNull;
  }
  char firstLetter = tolower(desc[0]);
  switch (firstLetter)
  {
    case 'a':
    case 'e':
    case 'i':
    case 'o':
    case 'u':
      return articleAn;
    default:
      return articleA;
  }
}

std::string const& getDefArt(std::string const& desc)
{
  static std::string const articleThe = "the";
  return articleThe;
}

