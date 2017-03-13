#include "stdafx.h"

#include "StringTransforms.h"

#include "Entity.h"
#include "EntityId.h"
#include "IStringDictionary.h"
#include "Service.h"

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

  std::string make_string(EntityId subject, EntityId object, std::string pattern, std::vector<std::string> optional_strings)
  {
    std::string new_string = replace_tokens(pattern,
                                            [&](std::string token) -> std::string
    {
      if (token == "are")
      {
        return subject->choose_verb(tr("VERB_BE_2"), tr("VERB_BE_3"));
      }
      if (token == "were")
      {
        return subject->choose_verb(tr("VERB_BE_P2"), tr("VERB_BE_P3"));
      }
      if (token == "do")
      {
        return subject->choose_verb(tr("VERB_DO_2"), tr("VERB_DO_3"));
      }
      if (token == "get")
      {
        return subject->choose_verb(tr("VERB_GET_2"), tr("VERB_GET_3"));
      }
      if (token == "have")
      {
        return subject->choose_verb(tr("VERB_HAVE_2"), tr("VERB_HAVE_3"));
      }
      if (token == "seem")
      {
        return subject->choose_verb(tr("VERB_SEEM_2"), tr("VERB_SEEM_3"));
      }
      if (token == "try")
      {
        return subject->choose_verb(tr("VERB_TRY_2"), tr("VERB_TRY_3"));
      }

      if ((token == "foo_is") || (token == "foois"))
      {
        return object->choose_verb(tr("VERB_BE_2"), tr("VERB_BE_3"));
      }
      if ((token == "foo_has") || (token == "foohas"))
      {
        return object->choose_verb(tr("VERB_HAVE_2"), tr("VERB_HAVE_3"));
      }

      if ((token == "the_foo") || (token == "thefoo"))
      {
        return object->get_identifying_string(ArticleChoice::Definite);
      }

      if ((token == "the_foos_location") || (token == "thefooslocation"))
      {
        return object->getLocation()->get_identifying_string(ArticleChoice::Definite);
      }

      if (token == "fooself")
      {
        return object->get_self_or_identifying_string(subject, ArticleChoice::Definite);
      }

      if ((token == "foo_pro_sub") || (token == "fooprosub"))
      {
        return object->get_subject_pronoun();
      }

      if ((token == "foo_pro_obj") || (token == "fooproobj"))
      {
        return object->get_object_pronoun();
      }

      if (token == "you")
      {
        return subject->get_subject_you_or_identifying_string();
      }
      if ((token == "you_pro_sub") || (token == "youprosub"))
      {
        return subject->get_subject_pronoun();
      }
      if ((token == "you_pro_obj") || (token == "youproobj"))
      {
        return subject->get_object_pronoun();
      }
      if (token == "yourself")
      {
        return subject->get_reflexive_pronoun();
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
        return subject->get_possessive_of(arg);
      }

      return "[" + token + "(" + arg + ")" + "]";
    },
      [&](std::string token) -> bool
    {
      if ((token == "cv") || (token == "subjcv") || (token == "subj_cv"))
      {
        return subject->is_third_person();
      }
      if ((token == "objcv") || (token == "obj_cv") || (token == "foocv") || (token == "foo_cv"))
      {
        return object->is_third_person();
      }
      if ((token == "is_player") || (token == "isplayer"))
      {
        return subject->is_player();
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

  std::string make_string(EntityId subject, EntityId object, std::string pattern)
  {
    return make_string(subject, object, pattern, {});
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
    return make_string(subject, object, tr(key), {});
  }

  std::string maketr(EntityId subject, EntityId object, std::string key, std::vector<std::string> optional_strings)
  {
    return make_string(subject, object, tr(key), optional_strings);
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
  /// and underscores only.
  /// Strings `yyy` or `zzz` can be blank, and can consist of any characters
  /// other than ":" for string `yyy` or ")" for string `zzz`.
  ///
  /// The string `$$` is recognized as a single `$` instead of being parsed as a
  /// token.
  ///
  /// In all cases, a backslash can be used to escape any character and prevent
  /// it from being recognized as a token delimiter.
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

    LOG(TRACE) << "Replacing tokens in: \"" << str << "\"";

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
              LOG(TRACE) << "Found token: \"" << token_name << "\"";
              std::transform(token_name.begin(), token_name.end(), token_name.begin(), ::towlower);
              if (*loc == '(')
              {
                token_argument.clear();
                state = TokenizerState::ParsingTokenArgument;
                current_string = &out_str;
              }
              else
              {
                token_result = token_functor(token_name);
                LOG(TRACE) << "Replacing token with: \"" << token_result << "\"";
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
            LOG(TRACE) << "Found token argument: \"" << token_argument << "\"";
            token_result = token_argument_functor(token_name, token_argument);
            LOG(TRACE) << "Replacing token with: \"" << token_result << "\"";
            out_str += token_result;

            if (*loc != L'\0')
            {
              out_str += *loc;
            }
            state = TokenizerState::Text;
            current_string = &out_str;
          }
          else
          {
            token_name += *loc;
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
          if (*loc == ':')
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
          if (*loc == ')')
          {
            LOG(TRACE) << "Found selector 'false' string: \"" << selector_false << "\"";
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
          LOG(WARNING) << "Unknown tokenizer state " << state;
          state = TokenizerState::Text;
          current_string = &out_str;
          break;
      } // end switch
      ++loc;
    } // end while

      LOG(TRACE) << "String is now: \"" << out_str << "\"";

    if (state != TokenizerState::Text)
    {
      LOG(WARNING) << "End of string while in tokenizer state " << state;
    }

    return out_str;
  }
} // end namespace