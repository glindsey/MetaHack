#include "stdafx.h"

#include "Gender.h"

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
  static std::string const articleAn = std::string("an");
  static std::string const articleA = std::string("a");
  static std::string const articleNull = std::string("");

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