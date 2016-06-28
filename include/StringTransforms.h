#pragma once

#include "stdafx.h"

#include "common_functions.h"

#include <boost/tokenizer.hpp>

/// Enum used for the replace_tokens state machine.
enum class TokenizerState
{
  Text,               ///< Just copying text as normal.
  ParsingToken,       ///< Parsing a token name.
  ParsingChoice,      ///< Parsing a choice selector.
  ParsingChoiceTrue,  ///< Parsing the "true" portion of a choice selector.
  ParsingChoiceFalse, ///< Parsing the "false" portion of a choice selector.
  ParsingChoiceError, ///< State when the selector contained an invalid character.
  MemberCount
};

inline std::ostream& operator<<(std::ostream& os, TokenizerState state)
{
  switch (state)
  {
    case TokenizerState::Text: os << "Text"; break;
    case TokenizerState::ParsingToken: os << "ParsingToken"; break;
    case TokenizerState::ParsingChoice: os << "ParsingChoice"; break;
    case TokenizerState::ParsingChoiceTrue: os << "ParsingChoiceTrue"; break;
    case TokenizerState::ParsingChoiceFalse: os << "ParsingChoiceFalse"; break;
    case TokenizerState::ParsingChoiceError: os << "ParsingChoiceError"; break;
    default: os << "??? (" << static_cast<int>(state) << ")";
  }

  return os;
}

/// Find all occurrence of tokens, e.g. `$xxx` or `$(xxx?yyy:zzz)`.
///
/// For tokens of form `$xxx`:
///   The passed-in functor `token_functor` is called with the string `xxx` as
/// an argument, and returns another string to replace it with.
///
/// For tokens of form `$(xxx?yyy:zzz)`:
///   The passed-in functor `choose_functor` is called with the string `xxx` as
/// an argument. If the functor returns true, the token will be replaced by the
/// string `yyy`; otherwise, it will be replaced by the string `zzz`.
///
/// String `xxx` must not be blank, and must consist of alphanumeric characters
/// only.
/// Strings `yyy` or `zzz` can be blank, and can consist of any characters
/// other than ":" for string `yyy` or ")" for string `zzz`.
///
/// The string `$$` is recognized as a single `$` instead of being parsed as a
/// token.
///
/// In all cases, a backslash can be used to escape any character and prevent
/// it from being recognized as a token delimiter.
inline StringDisplay replace_tokens(StringDisplay str,
                                    std::function<StringDisplay(StringDisplay)> token_functor,
                                    std::function<bool(StringDisplay)> choose_functor)
{
  TokenizerState state = TokenizerState::Text;
  StringDisplay out_str;
  StringDisplay token_name;
  StringDisplay token_result;
  StringDisplay selector_true;
  StringDisplay selector_false;
  StringDisplay discard_string;
  StringDisplay* current_string = &out_str;

  LOG(TRACE) << "Replacing tokens in: \"" << str << "\"";

  str += L'\0';

  auto loc = str.begin();

  while (loc != str.end())
  {
    // Handle characters escaped with a backslash.
    if (*loc == '\\')
    {
      ++loc;
      if (loc == str.end()) break;
      
      *current_string += *loc;

      ++loc;
      if (loc == str.end()) break;
    }

    switch (state)
    {
      case TokenizerState::Text:

        if (*loc == L'$')
        {
          state = TokenizerState::ParsingToken;
          current_string = &token_name;
          token_name.clear();
        }
        else if (*loc != L'\0')
        {
          out_str += *loc;
        }
        break;

      case TokenizerState::ParsingToken:
        if (token_name.empty())
        {
          if (*loc == L'$')
          {
            out_str += L'$';
            state = TokenizerState::Text;
            current_string = &out_str;
            break;
          }
          else if (*loc == L'(')
          {
            state = TokenizerState::ParsingChoice;
            current_string = &token_name;
            token_name.clear();
            break;
          }
        }

        if (iswalpha(*loc))
        {
          token_name += *loc;
        }
        else
        {
          if (!token_name.empty())
          {
            LOG(TRACE) << "Found token: \"" << token_name << "\"";
            std::transform(token_name.begin(), token_name.end(), token_name.begin(), ::towlower);
            token_result = token_functor(token_name);
            LOG(TRACE) << "Replacing token with: \"" << token_result << "\"";
            out_str += token_result;

            if (*loc != L'\0')
            {
              out_str += *loc;
            }
          }
          state = TokenizerState::Text;
          current_string = &out_str;
        }
        break;

      case TokenizerState::ParsingChoice:
        if (iswalpha(*loc))
        {
          token_name += *loc;
        }
        else if (*loc == L'?')
        {
          if (!token_name.empty())
          {
            LOG(TRACE) << "Found selector token: \"" << token_name << "\"";
            std::transform(token_name.begin(), token_name.end(), token_name.begin(), ::towlower);
            selector_true.clear();
            state = TokenizerState::ParsingChoiceTrue;
            current_string = &selector_true;
          }
          else
          {
            LOG(WARNING) << "Selector name empty, skipping token";
            state = TokenizerState::ParsingChoiceError;
            current_string = &discard_string;
          }
        }
        else
        {
          LOG(WARNING) << "Invalid character \"" << *loc << "\" seen in selector name \"" << token_name << "\", skipping token";
          state = TokenizerState::ParsingChoiceError;
          current_string = &discard_string;
        }
        break;

      case TokenizerState::ParsingChoiceTrue:
        if (*loc == L':')
        {
          LOG(TRACE) << "Found selector 'true' string: \"" << selector_true << "\"";
          selector_false.clear();
          state = TokenizerState::ParsingChoiceFalse;
          current_string = &selector_false;
        }
        else
        {
          selector_true += *loc;
        }
        break;

      case TokenizerState::ParsingChoiceFalse:
        if (*loc == L')')
        {
          LOG(TRACE) << "Found selector 'false' string: \"" << selector_false << "\"";
          bool result = choose_functor(token_name);
          token_result = (result ? selector_true : selector_false);
          LOG(TRACE) << "Replacing token with: \"" << token_result << "\"";
          out_str += (result ? selector_true : selector_false);
          state = TokenizerState::Text;
          current_string = &out_str;
        }
        else
        {
          selector_false += *loc;
        }
        break;

      case TokenizerState::ParsingChoiceError:
        if (*loc == L')')
        {
          state = TokenizerState::Text;
          current_string = &out_str;
        }
        break;

      default:
        LOG(WARNING) << "Unknown tokenizer state " << state;
        state = TokenizerState::Text;
        current_string = &out_str;
        break;

    } // end switch
    ++loc;

  } // end while

  LOG(TRACE) << "String is now: \"" << str << "\"";

  if (state != TokenizerState::Text)
  {
    LOG(WARNING) << "End of string while in tokenizer state " << state;
  }

  return out_str;
}