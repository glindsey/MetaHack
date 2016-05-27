#include "stdafx.h"

#include "Gender.h"

StringDisplay const subjectPronounArray[] =
{
  L"it",
  L"he",
  L"she",
  L"ze",
  L"ey",
  L"I",
  L"you",
  L"they",
  L"this one",
  L"something",
  L"someone"
};

StringDisplay const objectPronounArray[] =
{
  L"it",
  L"him",
  L"her",
  L"hir",
  L"em",
  L"me",
  L"you",
  L"them",
  L"this one",
  L"something",
  L"someone"
};

StringDisplay const reflexivePronounArray[] =
{
  L"itself",
  L"himself",
  L"herself",
  L"hirself",
  L"emself",
  L"myself",
  L"yourself",
  L"themselves",
  L"itself",
  L"itself",
  L"him/herself"
};

StringDisplay const possessiveAdjectiveArray[] =
{
  L"its",
  L"his",
  L"her",
  L"hir",
  L"eir",
  L"my",
  L"your",
  L"their",
  L"its",
  L"its",
  L"his/her"
};

StringDisplay const possessivePronounArray[] =
{
  L"its",
  L"his",
  L"hers",
  L"hirs",
  L"eirs",
  L"mine",
  L"yours",
  L"theirs",
  L"this one's",
  L"its",
  L"his/hers"
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
  static StringDisplay const articleAn = L"an";
  static StringDisplay const articleA = L"a";
  static StringDisplay const articleNull = L"";

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