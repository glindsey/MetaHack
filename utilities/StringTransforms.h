#pragma once

#include "stdafx.h"

#include "utilities/CommonFunctions.h"

#include <boost/tokenizer.hpp>

// Forward declarations
class EntityId;

namespace StringTransforms
{
  /// Enum used for the replace_tokens state machine.
  enum class TokenizerState
  {
    Text,                 ///< Just copying text as normal.
    ParsingToken,         ///< Parsing a token name.
    ParsingTokenArgument, ///< Parsing a token argument.
    ParsingChoice,        ///< Parsing a choice selector.
    ParsingChoiceTrue,    ///< Parsing the "true" portion of a choice selector.
    ParsingChoiceFalse,   ///< Parsing the "false" portion of a choice selector.
    ParsingChoiceError,   ///< State when the selector contained an invalid character.
    MemberCount
  };

  std::ostream& operator<<(std::ostream& os, TokenizerState state);

  std::string makeString(EntityId subject, EntityId object, std::string pattern, std::vector<std::string> optional_strings);
  std::string makeString(EntityId subject, EntityId object, std::string pattern);
  std::string make_string_numerical_tokens_only(std::string pattern, std::vector<std::string> optional_strings);
  std::string makeTr(EntityId subject, EntityId object, std::string key);
  std::string makeTr(EntityId subject, EntityId object, std::string key, std::vector<std::string> optional_strings);
  
  std::string replace_tokens(std::string str,
                             std::function<std::string(std::string)> token_functor,
                             std::function<std::string(std::string, std::string)> token_argument_functor,
                             std::function<bool(std::string)> choose_functor);

  std::string remove_extra_whitespace_from(std::string input);

} // end namespace