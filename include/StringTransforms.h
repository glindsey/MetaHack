#pragma once

#include "stdafx.h"

#include "common_functions.h"

#include <boost/tokenizer.hpp>

/// Find and replace all occurrences of a substring with another string.
/// @todo TEST ME
inline StringDisplay find_and_replace(StringDisplay str, StringDisplay find_str, StringDisplay replace_str)
{
  size_t loc = 0;
  while (loc != str.length())
  {
    loc = str.find(find_str, loc);
    str = str.substr(0, loc - 1) + replace_str + str.substr(loc + find_str.length());
    loc += find_str.length();
  }
  return str;
}

/// Find all occurrence of tokens, e.g. $xxx. 
/// The passed-in functor is called with the string "xxx" as an argument,
/// and returns another string to replace it with.
/// @warning This function WILL recursively expand tokens, so be careful
///          not to create an infinite loop.
/// @todo Get this to replace "$$" with "$". Right now this won't work.
inline StringDisplay replace_tokens(StringDisplay str,
                                    std::function<StringDisplay(StringDisplay)> functor)
{
  LOG(INFO) << "Replacing tokens in: \"" << str << "\"";

  auto loc = str.begin();

  while (loc != str.end())
  {
    loc = std::find(str.begin(), str.end(), L'$');

    if (loc != str.end())
    {
        auto token_loc = loc + 1;
        auto token_end_loc = std::find_if(token_loc, str.end(), [](wchar_t ch)
        {
            return iswspace(ch);
        });

        StringDisplay token_str{ token_loc, token_end_loc };
        LOG(INFO) << "Found token: \"" << token_str << "\"";

        std::transform(token_str.begin(), token_str.end(), token_str.begin(), ::towlower);

        StringDisplay replace_str = functor(token_str);

        LOG(INFO) << "Replacing with: \"" << replace_str << "\"";

        auto new_str = StringDisplay(str.begin(), loc) + replace_str;
        if (token_end_loc != str.end())
        {
            new_str += StringDisplay(token_end_loc + 1, str.end());
        }
        str = new_str;

        LOG(INFO) << "String is now: \"" << str << "\"";
    }
  }

  return str;
}

/// Find all occurrences of "choose" tokens, e.g. $(xxx?yyy:zzz). 
/// The passed-in functor is called with the string "xxx" as an argument.
/// The entire thing is replaced with "yyy" if the functor returns true, or
/// "zzz" if the functor returns false.
/// @todo FIX ME. This is definitely incorrect.
inline StringDisplay replace_choose_tokens(StringDisplay str,
                                           std::function<bool(StringDisplay)> functor)
{
  StringDisplay original_str = str;
  auto loc = str.begin();
  while (loc != str.end())
  {
    StringDisplay token_start_str = L"$(";
    loc = std::search(str.begin(), str.end(), token_start_str.begin(), token_start_str.end());
    auto token_loc = loc + 2;
    auto left_loc = std::find(token_loc, str.end(), L'?') + 1;
    auto right_loc = std::find(left_loc, str.end(), L':') + 1;
    auto close_loc = std::find(right_loc, str.end(), L')');
    if ((token_loc == str.end()) ||
        (left_loc == str.end()) ||
        (right_loc == str.end()) ||
        (close_loc == str.end()))
    {
      throw std::runtime_error(("Malformed choose token in " + wstring_to_utf8(original_str)).c_str());
    }

    StringDisplay token_str{ token_loc, left_loc - 1 };
    std::transform(token_str.begin(), token_str.end(), token_str.begin(), ::towlower);
    StringDisplay left_str{ left_loc, right_loc - 1 };
    StringDisplay right_str{ right_loc, close_loc - 1 };

    StringDisplay replace_str = functor(token_str) ? left_str : right_str;

    str = StringDisplay(str.begin(), loc - 1) + replace_str + StringDisplay(close_loc + 1, str.end());
  }

  return str;
}

