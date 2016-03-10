#include "stdafx.h"

#include "Gender.h"

StringDisplay const subjectPronounArray[] =
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

StringDisplay const objectPronounArray[] =
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

StringDisplay const reflexivePronounArray[] =
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

StringDisplay const possessiveAdjectiveArray[] =
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

StringDisplay const possessivePronounArray[] =
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

StringDisplay const& getSubjPro(Gender gender)
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

StringDisplay const& getObjPro(Gender gender)
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

StringDisplay const& getRefPro(Gender gender)
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

StringDisplay const& getPossAdj(Gender gender)
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

StringDisplay const& getPossPro(Gender gender)
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

StringDisplay const& getIndefArt(StringDisplay const& desc)
{
  static StringDisplay const articleAn = "an";
  static StringDisplay const articleA = "a";
  static StringDisplay const articleNull = "";

  if (desc.isEmpty())
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