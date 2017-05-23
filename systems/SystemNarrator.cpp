#include "systems/SystemNarrator.h"

#include "entity/Entity.h"
#include "entity/EntityId.h"
#include "Service.h"
#include "services/IConfigSettings.h"
#include "services/IGameRules.h"
#include "services/IStringDictionary.h"
#include "utilities/StringTransforms.h"

SystemNarrator::SystemNarrator(ComponentGlobals const& globals,
                               ComponentMap<std::string> const& category,
                               ComponentMap<ComponentGender> const& gender,
                               ComponentMap<ComponentHealth> const& health,
                               ComponentMap<ComponentPosition> const& position,
                               ComponentMap<std::string> const& properName,
                               ComponentMap<unsigned int> const& quantity) :
  SystemCRTP<SystemNarrator>({}),
  m_category{ category },
  m_gender{ gender },
  m_globals{ globals },
  m_health{ health },
  m_position{ position },
  m_properName{ properName },
  m_quantity{ quantity }
{}

SystemNarrator::~SystemNarrator()
{}

std::ostream& operator<<(std::ostream& os, SystemNarrator::TokenizerState state)
{
  switch (state)
  {
  case SystemNarrator::TokenizerState::Text: os << "Text"; break;
  case SystemNarrator::TokenizerState::ParsingToken: os << "ParsingToken"; break;
  case SystemNarrator::TokenizerState::ParsingChoice: os << "ParsingChoice"; break;
  case SystemNarrator::TokenizerState::ParsingChoiceTrue: os << "ParsingChoiceTrue"; break;
  case SystemNarrator::TokenizerState::ParsingChoiceFalse: os << "ParsingChoiceFalse"; break;
  case SystemNarrator::TokenizerState::ParsingChoiceError: os << "ParsingChoiceError"; break;
  default: os << "??? (" << static_cast<int>(state) << ")";
  }

  return os;
}

bool SystemNarrator::isThirdPerson(EntityId id) const
{
  return !((m_globals.player() == id) || (m_quantity.valueOr(id, 1) > 1));
}

std::string const& SystemNarrator::chooseVerb(EntityId id,
                                              std::string const& verb12,
                                              std::string const& verb3) const
{
  return isThirdPerson(id) ? verb3 : verb12;
}

std::string const& SystemNarrator::getSubjectPronoun(EntityId id) const
{
  return getSubjPro(getGenderOrYou(id));
}

std::string const& SystemNarrator::getObjectPronoun(EntityId id) const
{
  return getObjPro(getGenderOrYou(id));
}

std::string const& SystemNarrator::getReflexivePronoun(EntityId id) const
{
  return getRefPro(getGenderOrYou(id));
}

std::string const& SystemNarrator::getPossessiveAdjective(EntityId id) const
{
  return getPossAdj(getGenderOrYou(id));
}

std::string const& SystemNarrator::getPossessivePronoun(EntityId id) const
{
  return getPossPro(getGenderOrYou(id));
}

Gender SystemNarrator::getGenderOrYou(EntityId id) const
{
  if (m_globals.player() == id)
  {
    return Gender::SecondPerson;
  }
  else
  {
    if (m_quantity.valueOr(id, 1) > 1)
    {
      return Gender::Plural;
    }
    else
    {
      return m_gender.valueOrDefault(id).gender();
    }
  }
}

std::string SystemNarrator::getSubjectiveString(EntityId id,
                                                ArticleChoice articles) const
{
  std::string str;

  if (m_globals.player() == id)
  {
    if (m_health.existsFor(id) &&
        !m_health.of(id).isDead())
    {
      str = tr("PRONOUN_SUBJECT_YOU");
    }
    else
    {
      str = makeStringNumsOnly(tr("PRONOUN_POSSESSIVE_YOU"), { tr("NOUN_CORPSE") });
    }
  }
  else
  {
    str = getDescriptiveString(id, articles);
  }

  return str;
}

std::string SystemNarrator::getObjectiveString(EntityId id, 
                                               ArticleChoice articles) const
{
  std::string str;

  if (m_globals.player() == id)
  {
    if (m_health.existsFor(id) &&
        !m_health.of(id).isDead())
    {
      str = tr("PRONOUN_OBJECT_YOU");
    }
    else
    {
      str = makeStringNumsOnly(tr("PRONOUN_POSSESSIVE_YOU"), { tr("NOUN_CORPSE") });
    }
  }
  else
  {
    str = getDescriptiveString(id, articles);
  }

  return str;
}

std::string SystemNarrator::getReflexiveString(EntityId id, 
                                               EntityId other, 
                                               ArticleChoice articles) const
{
  if (other == id)
  {
    return getReflexivePronoun(id);
  }

  return getDescriptiveString(id, articles);
}

std::string SystemNarrator::getDescriptiveString(EntityId id,
                                                 ArticleChoice articles,
                                                 UsePossessives possessives) const
{
  auto& config = Service<IConfigSettings>::get();

  EntityId location = (m_position.existsFor(id) ? m_position.of(id).parent() : EntityId::Mu());
  unsigned int quantity = m_quantity.valueOr(id, 1);

  std::string name;

  bool owned;

  std::string debug_prefix;
  if (config.get("debug-show-thing-ids") == true)
  {
    debug_prefix = "(#" + std::to_string(id) + ") ";
  }

  std::string adjectives;
  std::string noun;
  std::string description;
  std::string suffix;

  owned = m_health.existsFor(location);
  adjectives = getDisplayAdjectives(id);

  if (quantity == 1)
  {
    noun = getDisplayName(id);

    if (owned && (possessives == UsePossessives::Yes))
    {
      description = location->getPossessiveString(noun, adjectives);
    }
    else
    {
      if (articles == ArticleChoice::Definite)
      {
        description = tr("ARTICLE_DEFINITE") + " " + adjectives + " " + noun;
      }
      else
      {
        description = getIndefArt(noun) + " " + adjectives + " " + noun;
      }
    }

    if (m_properName.existsFor(id))
    {
      auto properName = m_properName.of(id);
      if (!properName.empty())
      {
        suffix = tr("VERB_NAME_PP") + " " + properName;
      }
    }
  }
  else
  {
    noun = std::to_string(m_quantity.valueOr(id, 1)) + " " + getDisplayPlural(id);

    if (owned && (possessives == UsePossessives::Yes))
    {
      description = location->getPossessiveString(noun, adjectives);
    }
    else
    {
      if (articles == ArticleChoice::Definite)
      {
        description = tr("ARTICLE_DEFINITE") + " " + adjectives + " " + noun;
      }
      else
      {
        description = adjectives + " " + noun;
      }
    }
  }

  name = makeStringNumsOnly(tr("PATTERN_DISPLAY_NAME"), { debug_prefix,description,suffix });

  return name;
}

std::string SystemNarrator::getPossessiveString(EntityId id, std::string owned, std::string adjectives) const
{
  if (m_globals.player() == id)
  {
    return makeStringNumsOnly(tr("PRONOUN_POSSESSIVE_YOU"), { adjectives, owned });
  }
  else
  {
    return makeStringNumsOnly(tr("PATTERN_POSSESSIVE"), {
      getDescriptiveString(id, ArticleChoice::Definite, UsePossessives::No),
      adjectives,
      owned
    });
  }
}

std::string SystemNarrator::getDisplayAdjectives(EntityId id) const
{
  std::string adjectives;

  if (m_health.existsFor(id) && m_health.of(id).isDead())
  {
    adjectives += tr("ADJECTIVE_DEAD");
  }

  /// @todo Implement more adjectives.

  return adjectives;
}

json const& SystemNarrator::getCategoryData(EntityId id) const
{
  return Service<IGameRules>::get().category(m_category.valueOr(id, ""));
}

/// @todo Figure out how to cleanly localize this.
std::string SystemNarrator::getDisplayName(EntityId id) const
{
  return getCategoryData(id).value("name", std::string());
}

/// @todo Figure out how to cleanly localize this.
std::string SystemNarrator::getDisplayPlural(EntityId id) const
{
  return getCategoryData(id).value("plural", std::string());
}

std::string SystemNarrator::makeString(EntityId subject,
                                       EntityId object,
                                       std::string pattern,
                                       std::vector<std::string> optionalStrings) const
{
  std::string new_string = replaceTokens(pattern,
                                         [&](std::string token) -> std::string
  {
    if (token == "are")
    {
      return chooseVerb(subject, tr("VERB_BE_2"), tr("VERB_BE_3"));
    }
    if (token == "were")
    {
      return chooseVerb(subject, tr("VERB_BE_P2"), tr("VERB_BE_P3"));
    }
    if (token == "do")
    {
      return chooseVerb(subject, tr("VERB_DO_2"), tr("VERB_DO_3"));
    }
    if (token == "get")
    {
      return chooseVerb(subject, tr("VERB_GET_2"), tr("VERB_GET_3"));
    }
    if (token == "have")
    {
      return chooseVerb(subject, tr("VERB_HAVE_2"), tr("VERB_HAVE_3"));
    }
    if (token == "seem")
    {
      return chooseVerb(subject, tr("VERB_SEEM_2"), tr("VERB_SEEM_3"));
    }
    if (token == "try")
    {
      return chooseVerb(subject, tr("VERB_TRY_2"), tr("VERB_TRY_3"));
    }
    if ((token == "foo_is") || (token == "foois"))
    {
      return chooseVerb(subject, tr("VERB_BE_2"), tr("VERB_BE_3"));
    }
    if ((token == "foo_has") || (token == "foohas"))
    {
      return chooseVerb(subject, tr("VERB_HAVE_2"), tr("VERB_HAVE_3"));
    }
    if ((token == "the_foo") || (token == "thefoo"))
    {
      return getDescriptiveString(object, ArticleChoice::Definite);
    }
    if ((token == "the_foos_location") || (token == "thefooslocation"))
    {
      auto parent = (m_position.existsFor(object) ? m_position.of(object).parent : EntityId::Mu());
      return getDescriptiveString(parent, ArticleChoice::Definite);
    }
    if (token == "fooself")
    {
      return getReflexiveString(object, subject, ArticleChoice::Definite);
    }
    if ((token == "foo_pro_sub") || (token == "fooprosub"))
    {
      return getSubjectPronoun(object);
    }
    if ((token == "foo_pro_obj") || (token == "fooproobj"))
    {
      return getObjectPronoun(object);
    }
    if (token == "you")
    {
      return getSubjectiveString(subject);
    }
    if ((token == "you_pro_sub") || (token == "youprosub"))
    {
      return getSubjectPronoun(subject);
    }
    if ((token == "you_pro_obj") || (token == "youproobj"))
    {
      return getObjectPronoun(subject);
    }
    if (token == "yourself")
    {
      return getReflexivePronoun(subject);
    }

    // Check for a numerical token.
    try
    {
      unsigned int converted = static_cast<unsigned int>(std::stoi(token));

      // Check that the optional arguments are at least this size.
      if (converted < optionalStrings.size())
      {
        // Return the string passed in.
        return optionalStrings.at(converted);
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
      return getPossessiveString(subject, arg);
    }

    return "[" + token + "(" + arg + ")" + "]";
  },
    [&](std::string token) -> bool
  {
    if ((token == "cv") || (token == "subjcv") || (token == "subj_cv"))
    {
      return isThirdPerson(subject);
    }
    if ((token == "objcv") || (token == "obj_cv") || (token == "foocv") || (token == "foo_cv"))
    {
      return isThirdPerson(object);
    }
    if ((token == "isPlayer") || (token == "isplayer"))
    {
      return (m_globals.player() == subject);
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

std::string SystemNarrator::makeString(EntityId subject, EntityId object, std::string pattern) const
{
  return makeString(subject, object, pattern, {});
}

std::string SystemNarrator::makeStringNumsOnly(std::string pattern, std::vector<std::string> optionalStrings) const
{
  std::string new_string = replaceTokens(pattern,
                                         [&](std::string token) -> std::string
  {
    // Check for a numerical token.
    try
    {
      unsigned int converted = static_cast<unsigned int>(std::stoi(token));

      // Check that the optional arguments are at least this size.
      if (converted < optionalStrings.size())
      {
        // Return the string passed in.
        return optionalStrings.at(converted);
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

std::string SystemNarrator::makeTr(EntityId subject, 
                                   EntityId object, 
                                   std::string key) const
{
  return makeString(subject, object, tr(key), {});
}

std::string SystemNarrator::makeTr(EntityId subject, 
                                   EntityId object, 
                                   std::string key, 
                                   std::vector<std::string> optionalStrings) const
{
  return makeString(subject, object, tr(key), optionalStrings);
}

/// Find all occurrence of tokens, e.g. `$xxx` or `$(xxx?yyy:zzz)`.
///
/// For tokens of form `$xxx`:
///   The passed-in functor `tokenFunctor` is called with the string `xxx` as
/// an argument, and returns another string to replace it with.
///
/// For token of form `$xxx(yyy)`:
///   The passed-in functor `tokenArgumentFunctor` is called with the strings
/// `xxx` and `yyy` as arguments, and returns another string to replace the whole
/// thing with.
///
/// For tokens of form `$(xxx?yyy:zzz)`:
///   The passed-in functor `chooseFunctor` is called with the string `xxx` as
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
std::string SystemNarrator::replaceTokens(std::string str,
                                          std::function<std::string(std::string)> tokenFunctor,
                                          std::function<std::string(std::string, std::string)> tokenArgumentFunctor,
                                          std::function<bool(std::string)> chooseFunctor) const
{
  TokenizerState state = TokenizerState::Text;
  std::string outString;
  std::string tokenName;
  std::string tokenArgument;
  std::string tokenResult;
  std::string selectorTrue;
  std::string selectorFalse;
  std::string discardString;
  std::string* currentString = &outString;

  CLOG(TRACE, "Narrator") << "Replacing tokens in: \"" << str << "\"";

  str += '\0';

  auto loc = str.begin();

  while (loc != str.end())
  {
    // Handle characters escaped with a backslash.
    if (*loc == '\\')
    {
      ++loc;
      if (loc == str.end()) break;

      *currentString += *loc;

      ++loc;
      if (loc == str.end()) break;
    }

    switch (state)
    {
    case TokenizerState::Text:

      if (*loc == '$')
      {
        state = TokenizerState::ParsingToken;
        currentString = &tokenName;
        tokenName.clear();
      }
      else if (*loc != L'\0')
      {
        outString += *loc;
      }
      break;

    case TokenizerState::ParsingToken:
      if (tokenName.empty())
      {
        if (*loc == '$')
        {
          outString += '$';
          state = TokenizerState::Text;
          currentString = &outString;
          break;
        }
        else if (*loc == '(')
        {
          state = TokenizerState::ParsingChoice;
          currentString = &tokenName;
          tokenName.clear();
          break;
        }
      }

      if (iswalnum(*loc) || (*loc == '_'))
      {
        tokenName += *loc;
      }
      else
      {
        if (!tokenName.empty())
        {
          CLOG(TRACE, "Narrator") << "Found token: \"" << tokenName << "\"";
          std::transform(tokenName.begin(), tokenName.end(), tokenName.begin(), ::towlower);
          if (*loc == '(')
          {
            state = TokenizerState::ParsingTokenArgument;
            currentString = &tokenArgument;
            tokenArgument.clear();
          }
          else
          {
            tokenResult = tokenFunctor(tokenName);
            CLOG(TRACE, "Narrator") << "Replacing token with: \"" << tokenResult << "\"";
            outString += tokenResult;

            if (*loc != L'\0')
            {
              outString += *loc;
            }

            state = TokenizerState::Text;
            currentString = &outString;
          }
        }
        else
        {
          state = TokenizerState::Text;
          currentString = &outString;
        }
      }
      break;

    case TokenizerState::ParsingTokenArgument:
      if (*loc == ')')
      {
        CLOG(TRACE, "Narrator") << "Found token argument: \"" << tokenArgument << "\"";
        tokenResult = tokenArgumentFunctor(tokenName, tokenArgument);
        CLOG(TRACE, "Narrator") << "Replacing token with: \"" << tokenResult << "\"";
        outString += tokenResult;

        state = TokenizerState::Text;
        currentString = &outString;
      }
      else
      {
        tokenArgument += *loc;
      }
      break;

    case TokenizerState::ParsingChoice:
      if (iswalnum(*loc) || (*loc == '_'))
      {
        tokenName += *loc;
      }
      else if (*loc == '?')
      {
        if (!tokenName.empty())
        {
          CLOG(TRACE, "Narrator") << "Found selector token: \"" << tokenName << "\"";
          std::transform(tokenName.begin(), tokenName.end(), tokenName.begin(), ::towlower);
          selectorTrue.clear();
          state = TokenizerState::ParsingChoiceTrue;
          currentString = &selectorTrue;
        }
        else
        {
          CLOG(WARNING, "Narrator") << "Selector name empty, skipping token";
          state = TokenizerState::ParsingChoiceError;
          currentString = &discardString;
        }
      }
      else
      {
        CLOG(WARNING, "Narrator") << "Invalid character \"" << *loc << "\" seen in selector name \"" << tokenName << "\", skipping token";
        state = TokenizerState::ParsingChoiceError;
        currentString = &discardString;
      }
      break;

    case TokenizerState::ParsingChoiceTrue:
      if (*loc == ':')
      {
        CLOG(TRACE, "Narrator") << "Found selector 'true' string: \"" << selectorTrue << "\"";
        selectorFalse.clear();
        state = TokenizerState::ParsingChoiceFalse;
        currentString = &selectorFalse;
      }
      else
      {
        selectorTrue += *loc;
      }
      break;

    case TokenizerState::ParsingChoiceFalse:
      if (*loc == ')')
      {
        CLOG(TRACE, "Narrator") << "Found selector 'false' string: \"" << selectorFalse << "\"";
        bool result = chooseFunctor(tokenName);
        tokenResult = (result ? selectorTrue : selectorFalse);
        //LOG(TRACE) << "Replacing token with: \"" << tokenResult << "\"";
        outString += (result ? selectorTrue : selectorFalse);
        state = TokenizerState::Text;
        currentString = &outString;
      }
      else
      {
        selectorFalse += *loc;
      }
      break;

    case TokenizerState::ParsingChoiceError:
      if (*loc == ')')
      {
        state = TokenizerState::Text;
        currentString = &outString;
      }
      break;

    default:
      CLOG(WARNING, "Narrator") << "Unknown tokenizer state " << state;
      state = TokenizerState::Text;
      currentString = &outString;
      break;
    } // end switch
    ++loc;
  } // end while

  CLOG(TRACE, "Narrator") << "String is now: \"" << outString << "\"";

  if (state != TokenizerState::Text)
  {
    CLOG(WARNING, "Narrator") << "End of string while in tokenizer state " << state;
  }

  outString = StringTransforms::squishWhitespace(outString);
  boost::trim(outString);

  return outString;
}

std::string SystemNarrator::getVerb2(std::string verb) const
{
  auto& dict = Service<IStringDictionary>::get();
  return dict.get("VERB_" + verb + "_2");
}

std::string SystemNarrator::getVerb3(std::string verb) const
{
  auto& dict = Service<IStringDictionary>::get();
  return dict.get("VERB_" + verb + "_3");
}

std::string SystemNarrator::getVerbing(std::string verb) const
{
  auto& dict = Service<IStringDictionary>::get();
  return dict.get("VERB_" + verb + "_GER");
}

std::string SystemNarrator::getVerbed(std::string verb) const
{
  auto& dict = Service<IStringDictionary>::get();
  return dict.get("VERB_" + verb + "_P2");
}

std::string SystemNarrator::getVerbPP(std::string verb) const
{
  auto& dict = Service<IStringDictionary>::get();
  return dict.get("VERB_" + verb + "_PP");
}

std::string SystemNarrator::getVerbable(std::string verb) const
{
  auto& dict = Service<IStringDictionary>::get();
  return dict.get("VERB_" + verb + "_ABLE");
}

void SystemNarrator::doCycleUpdate()
{}

void SystemNarrator::setMapNVO(MapID newMap)
{}

bool SystemNarrator::onEvent(Event const & event)
{
  return false;
}

