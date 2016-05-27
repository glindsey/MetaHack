#ifndef ORDINAL_H_INCLUDED
#define ORDINAL_H_INCLUDED

namespace Ordinal
{
  inline StringDisplay get_abbrev(unsigned int number)
  {
    switch (number % 10)
    {
      case 1: return std::to_wstring(number) + L"st";
      case 2: return std::to_wstring(number) + L"nd";
      case 3: return std::to_wstring(number) + L"rd";
      default: return std::to_wstring(number) + L"th";
    }
  }

  inline StringDisplay get(unsigned int number)
  {
    switch (number)
    {
      case 0: return L"zeroth";
      case 1: return L"first";
      case 2: return L"second";
      case 3: return L"third";
      case 4: return L"fourth";
      case 5: return L"fifth";
      case 6: return L"sixth";
      case 7: return L"seventh";
      case 8: return L"eighth";
      case 9: return L"ninth";
      case 10: return L"tenth";
      case 11: return L"eleventh";
      case 12: return L"twelfth";
      case 13: return L"thirteenth";
      case 14: return L"fourteenth";
      case 15: return L"fifteenth";
      case 16: return L"sixteenth";
      case 17: return L"seventeenth";
      case 18: return L"eighteenth";
      case 19: return L"nineteenth";
      case 20: return L"twentieth";
      default: return get_abbrev(number);
    }
  }
}

#endif // ORDINAL_H_INCLUDED
