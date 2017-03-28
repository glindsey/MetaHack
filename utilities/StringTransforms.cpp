#include "stdafx.h"

#include "utilities/StringTransforms.h"

#include "entity/Entity.h"
#include "entity/EntityId.h"
#include "services/IStringDictionary.h"
#include "Service.h"

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

namespace StringTransforms
{
  std::ostream& operator<<(std::ostream& os, TokenizerState state)
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

  std::string makeString(EntityId subject, EntityId object, std::string pattern, std::vector<std::string> optional_strings)
  {
    std::string new_string = replace_tokens(pattern,
                                            [&](std::string token) -> std::string
    {
      if (token == "are")
      {
        return subject->chooseVerb(tr("VERB_BE_2"), tr("VERB_BE_3"));
      }
      if (token == "were")
      {
        return subject->chooseVerb(tr("VERB_BE_P2"), tr("VERB_BE_P3"));
      }
      if (token == "do")
      {
        return subject->chooseVerb(tr("VERB_DO_2"), tr("VERB_DO_3"));
      }
      if (token == "get")
      {
        return subject->chooseVerb(tr("VERB_GET_2"), tr("VERB_GET_3"));
      }
      if (token == "have")
      {
        return subject->chooseVerb(tr("VERB_HAVE_2"), tr("VERB_HAVE_3"));
      }
      if (token == "seem")
      {
        return subject->chooseVerb(tr("VERB_SEEM_2"), tr("VERB_SEEM_3"));
      }
      if (token == "try")
      {
        return subject->chooseVerb(tr("VERB_TRY_2"), tr("VERB_TRY_3"));
      }

      if ((token == "foo_is") || (token == "foois"))
      {
        return object->chooseVerb(tr("VERB_BE_2"), tr("VERB_BE_3"));
      }
      if ((token == "foo_has") || (token == "foohas"))
      {
        return object->chooseVerb(tr("VERB_HAVE_2"), tr("VERB_HAVE_3"));
      }

      if ((token == "the_foo") || (token == "thefoo"))
      {
        return object->getDescriptiveString(ArticleChoice::Definite);
      }

      if ((token == "the_foos_location") || (token == "thefooslocation"))
      {
        return object->getLocation()->getDescriptiveString(ArticleChoice::Definite);
      }

      if (token == "fooself")
      {
        return object->getReflexiveString(subject, ArticleChoice::Definite);
      }

      if ((token == "foo_pro_sub") || (token == "fooprosub"))
      {
        return object->getSubjectPronoun();
      }

      if ((token == "foo_pro_obj") || (token == "fooproobj"))
      {
        return object->getObjectPronoun();
      }

      if (token == "you")
      {
        return subject->getSubjectiveString();
      }
      if ((token == "you_pro_sub") || (token == "youprosub"))
      {
        return subject->getSubjectPronoun();
      }
      if ((token == "you_pro_obj") || (token == "youproobj"))
      {
        return subject->getObjectPronoun();
      }
      if (token == "yourself")
      {
        return subject->getReflexivePronoun();
      }

      // Check for a numerical token.
      try
      {
        unsigned int converted = static_cast<unsigned int>(std::stoi(token));

        // Check that the optional arguments are at least this size.
        if (converted < optional_strings.size())
        {
          // Return the string passed in.
          return optional_strings.at(converted);
        }
      }
      catch (std::invalid_argument&)
      {
        // Not a number, so bail.
      }
      catch (...)
      {
        // Throw anything else.
        throw;
      }

      // Nothing else matched, return default.
      return "[" + token + "]";
    },
                                            [&](std::string token, std::string arg) -> std::string
    {
      if (token == "your")
      {
        return subject->getPossessiveString(arg);
      }

      return "[" + token + "(" + arg + ")" + "]";
    },
      [&](std::string token) -> bool
    {
      if ((token == "cv") || (token == "subjcv") || (token == "subj_cv"))
      {
        return subject->isThirdPerson();
      }
      if ((token == "objcv") || (token == "obj_cv") || (token == "foocv") || (token == "foo_cv"))
      {
        return object->isThirdPerson();
      }
      if ((token == "isPlayer") || (token == "isplayer"))
      {
        return subject->isPlayer();
      }

      if (token == "true")
      {
        return true;
      }
      if (token == "false")
      {
        return false;
      }

      return true;
    });

    return new_string;
  }

  std::string makeString(EntityId subject, EntityId object, std::string pattern)
  {
    return makeString(subject, object, pattern, {});
  }

  std::string make_string_numerical_tokens_only(std::string pattern, std::vector<std::string> optional_strings)
  {
    std::string new_string = replace_tokens(pattern,
                                            [&](std::string token) -> std::string
    {
      // Check for a numerical token.
      try
      {
        unsigned int converted = static_cast<unsigned int>(std::stoi(token));

        // Check that the optional arguments are at least this size.
        if (converted < optional_strings.size())
        {
          // Return the string passed in.
          return optional_strings.at(converted);
        }
      }
      catch (std::invalid_argument&)
      {
        // Not a number, so bail.
      }
      catch (...)
      {
        // Throw anything else.
        throw;
      }

      // Nothing else matched, return default.
      return "[" + token + "]";
    },
                                            [&](std::string token, std::string arg) -> std::string
    {
      return "[" + token + "(" + arg + ")" + "]";
    },
      [&](std::string token) -> bool
    {
      if (token == "true")
      {
        return true;
      }
      if (token == "false")
      {
        return false;
      }

      return true;
    });

    return new_string;
  }

  std::string maketr(EntityId subject, EntityId object, std::string key)
  {
    return makeString(subject, object, tr(key), {});
  }

  std::string maketr(EntityId subject, EntityId object, std::string key, std::vector<std::string> optional_strings)
  {
    return makeString(subject, object, tr(key), optional_strings);
  }

  /// Find all occurrence of tokens, e.g. `$xxx` or `$(xxx?yyy:zzz)`.
  ///
  /// For tokens of form `$xxx`:
  ///   The passed-in functor `token_functor` is called with the string `xxx` as
  /// an argument, and returns another string to replace it with.
  ///
  /// For token of form `$xxx(yyy)`:
  ///   The passed-in functor `token_argument_functor` is called with the strings
  /// `xxx` and `yyy` as arguments, and returns another string to replace the whole
  /// thing with.
  ///
  /// For tokens of form `$(xxx?yyy:zzz)`:
  ///   The passed-in functor `choose_functor` is called with the string `xxx` as
  /// an argument. If the functor returns true, the token will be replaced by the
  /// string `yyy`; otherwise, it will be replaced by the string `zzz`.
  ///
  /// String `xxx` must not be blank, and must consist of alphanumeric characters
  /// and underscores only.
  /// Strings `yyy` or `zzz` can be blank, and can consist of any characters
  /// other than ":" for string `yyy` or ")" for string `zzz`.
  ///
  /// The string `$$` is recognized as a single `$` instead of being parsed as a
  /// token.
  ///
  /// In all cases, a backslash can be used to escape any character and prevent
  /// it from being recognized as a token delimiter.
  /// 
  /// The final string has all whitespace sequences reduced to a single space
  /// each, and has excess whitespace trimmed from the head and tail.
  std::string replace_tokens(std::string str,
                             std::function<std::string(std::string)> token_functor,
                             std::function<std::string(std::string, std::string)> token_argument_functor,
                             std::function<bool(std::string)> choose_functor)
  {
    TokenizerState state = TokenizerState::Text;
    std::string out_str;
    std::string token_name;
    std::string token_argument;
    std::string token_result;
    std::string selector_true;
    std::string selector_false;
    std::string discard_string;
    std::string* current_string = &out_str;

    CLOG(TRACE, "StringTransforms") << "Replacing tokens in: \"" << str << "\"";

    str += '\0';

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

          if (*loc == '$')
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
            if (*loc == '$')
            {
              out_str += '$';
              state = TokenizerState::Text;
              current_string = &out_str;
              break;
            }
            else if (*loc == '(')
            {
              state = TokenizerState::ParsingChoice;
              current_string = &token_name;
              token_name.clear();
              break;
            }
          }

          if (iswalnum(*loc) || (*loc == '_'))
          {
            token_name += *loc;
          }
          else
          {
            if (!token_name.empty())
            {
              CLOG(TRACE, "StringTransforms") << "Found token: \"" << token_name << "\"";
              std::transform(token_name.begin(), token_name.end(), token_name.begin(), ::towlower);
              if (*loc == '(')
              {
                state = TokenizerState::ParsingTokenArgument;
                current_string = &token_argument;
                token_argument.clear();
              }
              else
              {
                token_result = token_functor(token_name);
                CLOG(TRACE, "StringTransforms") << "Replacing token with: \"" << token_result << "\"";
                out_str += token_result;

                if (*loc != L'\0')
                {
                  out_str += *loc;
                }

                state = TokenizerState::Text;
                current_string = &out_str;
              }
            }
            else
            {
              state = TokenizerState::Text;
              current_string = &out_str;
            }
          }
          break;

        case TokenizerState::ParsingTokenArgument:
          if (*loc == ')')
          {
            CLOG(TRACE, "StringTransforms") << "Found token argument: \"" << token_argument << "\"";
            token_result = token_argument_functor(token_name, token_argument);
            CLOG(TRACE, "StringTransforms") << "Replacing token with: \"" << token_result << "\"";
            out_str += token_result;

            state = TokenizerState::Text;
            current_string = &out_str;
          }
          else
          {
            token_argument += *loc;
          }
          break;

        case TokenizerState::ParsingChoice:
          if (iswalnum(*loc) || (*loc == '_'))
          {
            token_name += *loc;
          }
          else if (*loc == '?')
          {
            if (!token_name.empty())
            {
              CLOG(TRACE, "StringTransforms") << "Found selector token: \"" << token_name << "\"";
              std::transform(token_name.begin(), token_name.end(), token_name.begin(), ::towlower);
              selector_true.clear();
              state = TokenizerState::ParsingChoiceTrue;
              current_string = &selector_true;
            }
            else
            {
              CLOG(WARNING, "StringTransforms") << "Selector name empty, skipping token";
              state = TokenizerState::ParsingChoiceError;
              current_string = &discard_string;
            }
          }
          else
          {
            CLOG(WARNING, "StringTransforms") << "Invalid character \"" << *loc << "\" seen in selector name \"" << token_name << "\", skipping token";
            state = TokenizerState::ParsingChoiceError;
            current_string = &discard_string;
          }
          break;

        case TokenizerState::ParsingChoiceTrue:
          if (*loc == ':')
          {
            CLOG(TRACE, "StringTransforms") << "Found selector 'true' string: \"" << selector_true << "\"";
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
          if (*loc == ')')
          {
            CLOG(TRACE, "StringTransforms") << "Found selector 'false' string: \"" << selector_false << "\"";
            bool result = choose_functor(token_name);
            token_result = (result ? selector_true : selector_false);
            //LOG(TRACE) << "Replacing token with: \"" << token_result << "\"";
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
          if (*loc == ')')
          {
            state = TokenizerState::Text;
            current_string = &out_str;
          }
          break;

        default:
          CLOG(WARNING, "StringTransforms") << "Unknown tokenizer state " << state;
          state = TokenizerState::Text;
          current_string = &out_str;
          break;
      } // end switch
      ++loc;
    } // end while

    CLOG(TRACE, "StringTransforms") << "String is now: \"" << out_str << "\"";

    if (state != TokenizerState::Text)
    {
      CLOG(WARNING, "StringTransforms") << "End of string while in tokenizer state " << state;
    }
    
    //boost::regex expr("\\w+");
    //std::string fmt(" ");
    //out_str = boost::regex_replace(out_str, expr, fmt, boost::match_default | boost::regex_constants::format_literal);
    out_str = remove_extra_whitespace_from(out_str);
    boost::trim(out_str);

    return out_str;
  }

  std::string remove_extra_whitespace_from(std::string input)
  {
    std::string output;
    std::unique_copy(input.begin(), input.end(),
                     std::back_insert_iterator<std::string>(output),
                     [](char a, char b) { return std::isspace(a) && std::isspace(b); });
    return output;
  }
} // end namespace