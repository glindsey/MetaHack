#ifndef ORDINAL_H_INCLUDED
#define ORDINAL_H_INCLUDED

namespace Ordinal
{
  inline std::string get_abbrev(unsigned int number)
  {
    char buf[4];
    snprintf(buf, 4, "%u", number);
    switch (number % 10)
    {
      case 1: return std::string(buf) + "st";
      case 2: return std::string(buf) + "nd";
      case 3: return std::string(buf) + "rd";
      default: return std::string(buf) + "th";
    }
  }

  inline std::string get(unsigned int number)
  {
    switch (number)
    {
      case 0: return "zeroth";
      case 1: return "first";
      case 2: return "second";
      case 3: return "third";
      case 4: return "fourth";
      case 5: return "fifth";
      case 6: return "sixth";
      case 7: return "seventh";
      case 8: return "eighth";
      case 9: return "ninth";
      case 10: return "tenth";
      case 11: return "eleventh";
      case 12: return "twelfth";
      case 13: return "thirteenth";
      case 14: return "fourteenth";
      case 15: return "fifteenth";
      case 16: return "sixteenth";
      case 17: return "seventeenth";
      case 18: return "eighteenth";
      case 19: return "nineteenth";
      case 20: return "twentieth";
      default: return get_abbrev(number);
    }
  }
}

#endif // ORDINAL_H_INCLUDED
