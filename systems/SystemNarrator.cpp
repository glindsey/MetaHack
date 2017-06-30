#include "stdafx.h"

#include "systems/SystemNarrator.h"

#include "components/ComponentBodyparts.h"
#include "components/ComponentGender.h"
#include "components/ComponentGlobals.h"
#include "components/ComponentHealth.h"
#include "components/ComponentMap.h"
#include "components/ComponentManager.h"
#include "components/ComponentPosition.h"
#include "entity/EntityId.h"
#include "services/Service.h"
#include "services/IConfigSettings.h"
#include "services/IGameRules.h"
#include "services/IStrings.h"
#include "utilities/JSONUtils.h"
#include "utilities/Ordinal.h"
#include "utilities/Shortcuts.h"
#include "utilities/StringTransforms.h"

namespace Systems
{
  Narrator::Narrator(Components::ComponentManager const& components) :
    CRTP<Narrator>({}),
    m_components{ components }
  {}

  Narrator::~Narrator()
  {}

  std::ostream& operator<<(std::ostream& os, Narrator::TokenizerState state)
  {
    switch (state)
    {
    case Narrator::TokenizerState::Text: os << "Text"; break;
    case Narrator::TokenizerState::ParsingToken: os << "ParsingToken"; break;
    case Narrator::TokenizerState::ParsingChoice: os << "ParsingChoice"; break;
    case Narrator::TokenizerState::ParsingChoiceTrue: os << "ParsingChoiceTrue"; break;
    case Narrator::TokenizerState::ParsingChoiceFalse: os << "ParsingChoiceFalse"; break;
    case Narrator::TokenizerState::ParsingChoiceError: os << "ParsingChoiceError"; break;
    default: os << "??? (" << static_cast<int>(state) << ")";
    }

    return os;
  }

  bool Narrator::isThirdPerson(EntitySet entities) const
  {
    if (entities.size() != 1) return true;
    EntityId entity = *(entities.cbegin());
    return !((m_components.globals.player() == entity) || (m_components.quantity.valueOr(entity, 1) > 1));
  }

  std::string const& Narrator::chooseVerb(EntitySet entities,
                                          std::string const& verb12,
                                          std::string const& verb3) const
  {
    return isThirdPerson(entities) ? verb3 : verb12;
  }

  std::string const& Narrator::getSubjectPronoun(EntitySet entities) const
  {
    return getSubjPro(getGenderOrYou(entities));
  }

  std::string const& Narrator::getObjectPronoun(EntitySet entities) const
  {
    return getObjPro(getGenderOrYou(entities));
  }

  std::string const& Narrator::getReflexivePronoun(EntitySet entities) const
  {
    return getRefPro(getGenderOrYou(entities));
  }

  std::string const& Narrator::getPossessiveAdjective(EntitySet entities) const
  {
    return getPossAdj(getGenderOrYou(entities));
  }

  std::string const& Narrator::getPossessivePronoun(EntitySet entities) const
  {
    return getPossPro(getGenderOrYou(entities));
  }

  Gender Narrator::getGenderOrYou(EntitySet entities) const
  {
    if (entities.size() != 1) return Gender::Plural;
    EntityId entity = *(entities.cbegin());

    if (m_components.globals.player() == entity)
    {
      return Gender::SecondPerson;
    }
    else
    {
      if (m_components.quantity.valueOr(entity, 1) > 1)
      {
        return Gender::Plural;
      }
      else
      {
        return m_components.gender.valueOrDefault(entity).gender();
      }
    }
  }

  std::string Narrator::getSubjectiveString(EntityId id,
                                            ArticleChoice articles,
                                            UsePossessives possessives) const
  {
    std::string str;

    if (m_components.globals.player() == id)
    {
      if (m_components.health.existsFor(id) &&
          !m_components.health.of(id).isDead())
      {
        str = tr("PRONOUN_SUBJECT_YOU");
      }
      else
      {
        str = makeStringTokensOnly(tr("PATTERN_POSSESSIVE_YOU"), { {"adjectives", ""}, {"noun", tr("NOUN_CORPSE")} });
      }
    }
    else
    {
      str = getDescriptiveString(id, articles, possessives);
    }

    return str;
  }

  std::string Narrator::getObjectiveString(EntityId id,
                                           ArticleChoice articles,
                                           UsePossessives possessives) const
  {
    std::string str;

    if (m_components.globals.player() == id)
    {
      if (m_components.health.existsFor(id) &&
          !m_components.health.of(id).isDead())
      {
        str = tr("PRONOUN_OBJECT_YOU");
      }
      else
      {
        str = str = makeStringTokensOnly(tr("PATTERN_POSSESSIVE_YOU"), { { "adjectives", "" },{ "noun", tr("NOUN_CORPSE") } });
      }
    }
    else
    {
      str = getDescriptiveString(id, articles, possessives);
    }

    return str;
  }

  std::string Narrator::getReflexiveString(EntityId id,
                                           EntityId other,
                                           ArticleChoice articles,
                                           UsePossessives possessives) const
  {
    if (id == other)
    {
      return getReflexivePronoun({ id });
    }

    return getDescriptiveString(id, articles, possessives);
  }

  std::string Narrator::getDescription(EntitySet entities,
                                       std::function<std::string(EntityId)> descriptionFunctor) const
  {
    size_t counter = 0;
    size_t numEntities = entities.size();
    std::string result;
    if (numEntities == 0)
    {
      result = tr("NOUN_NOTHING");
    }
    else
    {
      for (EntityId entity : entities)
      {
        result += descriptionFunctor(entity);
        ++counter;
        if (counter != numEntities)
        {
          result += (numEntities == 2) ? " " : ", ";
        }
        if (numEntities > 2 && counter == numEntities - 1) result = "and ";
      }
    }

    return result;
  }

  std::string Narrator::getDescriptiveString(EntityId id,
                                             ArticleChoice articles,
                                             UsePossessives possessives) const
  {
    auto& config = S<IConfigSettings>();

    EntityId location = (m_components.position.existsFor(id) ? m_components.position.of(id).parent() : EntityId::Void);
    unsigned int quantity = m_components.quantity.valueOr(id, 1);

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

    owned = m_components.health.existsFor(location);
    adjectives = getDisplayAdjectives(id);

    if (quantity == 1)
    {
      noun = getDisplayName(id);

      if (owned && (possessives == UsePossessives::Yes))
      {
        description = getPossessiveString(location, noun, adjectives);
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

      if (m_components.properName.existsFor(id))
      {
        auto properName = m_components.properName.of(id);
        if (!properName.empty())
        {
          suffix = tr("VERB_NAME_PP") + " " + properName;
        }
      }
    }
    else
    {
      noun = std::to_string(m_components.quantity.valueOr(id, 1)) + " " + getDisplayPlural(id);

      if (owned && (possessives == UsePossessives::Yes))
      {
        description = getPossessiveString(location, noun, adjectives);
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

    name = makeStringTokensOnly(tr("PATTERN_DISPLAY_NAME"),
    {
      { "debug", debug_prefix },
      { "description", description },
      { "suffix", suffix }
    });

    return name;
  }

  std::string Narrator::getPossessiveString(EntityId id, std::string owned, std::string adjectives) const
  {
    if (m_components.globals.player() == id)
    {
      return makeStringTokensOnly(tr("PATTERN_POSSESSIVE_YOU"),
      {
        { "adjectives", adjectives },
        { "noun", owned }
      });
    }
    else
    {
      return makeStringTokensOnly(tr("PATTERN_POSSESSIVE"),
      {
        { "subject", getDescriptiveString(id, ArticleChoice::Definite, UsePossessives::No) },
        { "adjectives", adjectives },
        { "noun", owned }
      });
    }
  }

  json const& Narrator::getCategoryData(EntityId id) const
  {
    return S<IGameRules>().categoryData(m_components.category.valueOr(id, ""));
  }

  std::string Narrator::getDisplayAdjectives(EntityId id) const
  {
    std::string adjectives;

    if (m_components.health.existsFor(id) && m_components.health.of(id).isDead())
    {
      adjectives += tr("ADJECTIVE_DEAD");
    }

    /// @todo Implement more adjectives.

    return adjectives;
  }

  /// @todo Figure out how to cleanly localize this.
  std::string Narrator::getDisplayName(EntityId id) const
  {
    return getCategoryData(id).value("name", std::string());
  }

  /// @todo Figure out how to cleanly localize this.
  std::string Narrator::getDisplayPlural(EntityId id) const
  {
    return getCategoryData(id).value("plural", std::string());
  }

  std::string Narrator::makeString(std::string pattern, json arguments) const
  {
    EntityId subject = arguments.value("subject", EntityId::Void);
    EntitySet objects = JSONUtils::getSet<EntityId>(arguments["object"]);
    EntityId target = arguments.value("target", EntityId::Void);
    std::string verb = arguments.value("verb", "");
    Direction targetDir = arguments.value("target-direction", Direction::None);

    std::string new_string = replaceTokens(pattern,
                                           [&](std::string token) -> std::string
    {
      if (token == "are")
      {
        return chooseVerb({ subject }, tr("VERB_BE_2"), tr("VERB_BE_3"));
      }
      if (token == "were")
      {
        return chooseVerb({ subject }, tr("VERB_BE_P2"), tr("VERB_BE_P3"));
      }
      if (token == "do")
      {
        return chooseVerb({ subject }, tr("VERB_DO_2"), tr("VERB_DO_3"));
      }
      if (token == "get")
      {
        return chooseVerb({ subject }, tr("VERB_GET_2"), tr("VERB_GET_3"));
      }
      if (token == "have")
      {
        return chooseVerb({ subject }, tr("VERB_HAVE_2"), tr("VERB_HAVE_3"));
      }
      if (token == "seem")
      {
        return chooseVerb({ subject }, tr("VERB_SEEM_2"), tr("VERB_SEEM_3"));
      }
      if (token == "try")
      {
        return chooseVerb({ subject }, tr("VERB_TRY_2"), tr("VERB_TRY_3"));
      }
      if ((token == "foo_is") || (token == "foois"))
      {
        return chooseVerb(objects, tr("VERB_BE_2"), tr("VERB_BE_3"));
      }
      if ((token == "foo_has") || (token == "foohas"))
      {
        return chooseVerb(objects, tr("VERB_HAVE_2"), tr("VERB_HAVE_3"));
      }
      if ((token == "the_foo") || (token == "thefoo"))
      {
        return getDescription(objects,
                              [&](EntityId entity) -> std::string
        {
          return getDescriptiveString(entity);
        });
      }
      if ((token == "the_foos_location") || (token == "thefooslocation"))
      {
        if (objects.size() < 1) return "";
        auto object = *(objects.cbegin());
        auto parent = (m_components.position.existsFor(object) ? m_components.position.of(object).parent() : EntityId::Void);
        return getDescriptiveString(parent, ArticleChoice::Definite);
      }
      if ((token == "the_target_thing") || (token == "thetargetthing"))
      {
        return getDescriptiveString(target);
      }
      if (token == "fooself")
      {
        if (objects.size() < 1) return "";
        auto object = *(objects.cbegin());
        return getReflexiveString(object, subject, ArticleChoice::Definite);
      }
      if ((token == "foo_pro_sub") || (token == "fooprosub"))
      {
        return getSubjectPronoun(objects);
      }
      if ((token == "foo_pro_obj") || (token == "fooproobj"))
      {
        return getObjectPronoun(objects);
      }
      if ((token == "foo_pro_ref") || (token == "fooproref"))
      {
        return getReflexivePronoun(objects);
      }
      if (token == "verb")
      {
        return getVerb2(verb);
      }
      if (token == "verb3")
      {
        return getVerb3(verb);
      }
      if (token == "verbed")
      {
        return getVerbed(verb);
      }
      if (token == "verbing")
      {
        return getVerbing(verb);
      }
      if ((token == "verb_pp") || (token == "verbpp"))
      {
        return getVerbPP(verb);
      }
      if (token == "cverb")
      {
        return (isThirdPerson({ subject }) ? getVerb3(verb) : getVerb2(verb));
      }
      if (token == "objcverb")
      {
        return (isThirdPerson(objects) ? getVerb3(verb) : getVerb2(verb));
      }
      if ((token == "a_subject_type") || (token == "asubjecttype"))
      {
        return getIndefArt(subject) + " " + getDisplayName(subject);
      }
      if (token == "you")
      {
        return getSubjectiveString(subject);
      }
      if ((token == "you_pro_sub") || (token == "youprosub"))
      {
        return getSubjectPronoun({ subject });
      }
      if ((token == "you_pro_obj") || (token == "youproobj"))
      {
        return getObjectPronoun({ subject });
      }
      if ((token == "your_location") || (token == "yourlocation"))
      {
        auto parent = (m_components.position.existsFor(subject) ? m_components.position.of(subject).parent() : EntityId::Void);
        return getDescriptiveString(parent, ArticleChoice::Indefinite);
      }
      if (token == "yourself")
      {
        return getReflexivePronoun({ subject });
      }
      if (token == "targdir")
      {
        std::stringstream ss;
        ss << targetDir;
        return ss.str();
      }

      // Check for another token. Substitute if it exists, return "[token]" if not.
      if (arguments.count(token) != 0 && !arguments[token].is_null())
      {
        return arguments[token];
      }
      else
      {
        return "[" + token + "]";
      }
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
        return isThirdPerson({ subject });
      }
      if ((token == "objcv") || (token == "obj_cv") || (token == "foocv") || (token == "foo_cv"))
      {
        return isThirdPerson(objects);
      }
      if ((token == "isPlayer") || (token == "isplayer"))
      {
        return (m_components.globals.player() == subject);
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

  std::string Narrator::makeStringTokensOnly(std::string pattern, json arguments) const
  {
    std::string new_string = replaceTokens(pattern,
                                           [&](std::string token) -> std::string
    {
      // Check for token. Substitute if it exists, return "[token]" if not.
      if (arguments.count(token) != 0 && !arguments[token].is_null())
      {
        return arguments[token];
      }
      else
      {
        return "[" + token + "]";
      }
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

  std::string Narrator::makeTr(std::string key, json arguments) const
  {
    return makeString(tr(key), arguments);
  }

  std::string Narrator::replaceTokens(std::string str,
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

  unsigned int Narrator::getBodypartNumber(EntityId id, BodyPart part) const
  {
    if (!m_components.bodyparts.existsFor(id)) return 0;
    auto& bodyparts = m_components.bodyparts.of(id);
    return bodyparts.typicalCount(part);
  }

  std::string Narrator::getBodypartName(EntityId id, BodyPart part) const
  {
    std::stringstream ss;
    ss << part;

    std::string fancyPartName = "NOUN_" + m_components.category.of(id) + "_" + ss.str();
    std::string partName = "NOUN_" + ss.str();

    boost::to_upper(fancyPartName);
    boost::to_upper(partName);

    bool fancyNameExists = S<IStrings>().contains(fancyPartName);

    return (fancyNameExists ? tr(fancyPartName) : tr(partName));
  }

  std::string Narrator::getBodypartPlural(EntityId id, BodyPart part) const
  {
    std::stringstream ss;
    ss << part;

    std::string fancyPartName = "NOUN_" + m_components.category.of(id) + "_" + ss.str() + "_PLURAL";
    std::string partName = "NOUN_" + ss.str() + "_PLURAL";

    boost::to_upper(fancyPartName);
    boost::to_upper(partName);

    bool fancyNameExists = S<IStrings>().contains(fancyPartName);

    return (fancyNameExists ? tr(fancyPartName) : tr(partName));
  }

  /// @todo Figure out how to localize this.
  std::string Narrator::getBodypartDescription(EntityId id, BodyLocation location)
  {
    uint32_t total_number = getBodypartNumber(id, location.part);
    std::string part_name = getBodypartName(id, location.part);
    std::string result;

    Assert("Narrator", location.number < total_number, "asked for bodypart " << location.number << " of " << total_number);
    switch (total_number)
    {
    case 0: // none of them!?  shouldn't occur!
      result = "non-existent " + part_name;
      CLOG(WARNING, "Narrator") << "Request for description of " << result << "!?";
      break;

    case 1: // only one of them
      result = part_name;
      break;

    case 2: // assume a right and left one.
      switch (location.number)
      {
      case 0:
        result = "right " + part_name;
        break;
      case 1:
        result = "left " + part_name;
        break;
      default:
        break;
      }
      break;

    case 3: // assume right, center, and left.
      switch (location.number)
      {
      case 0:
        result = "right " + part_name;
        break;
      case 1:
        result = "center " + part_name;
        break;
      case 2:
        result = "left " + part_name;
        break;
      default:
        break;
      }
      break;

    case 4: // Legs/feet assume front/rear, others assume upper/lower.
      if ((location.part == BodyPart::Leg) || (location.part == BodyPart::Foot))
      {
        switch (location.number)
        {
        case 0:
          result = "front right " + part_name;
          break;
        case 1:
          result = "front left " + part_name;
          break;
        case 2:
          result = "rear right " + part_name;
          break;
        case 3:
          result = "rear left " + part_name;
          break;
        default:
          break;
        }
      }
      else
      {
        switch (location.number)
        {
        case 0:
          result = "upper right " + part_name;
          break;
        case 1:
          result = "upper left " + part_name;
          break;
        case 2:
          result = "lower right " + part_name;
          break;
        case 3:
          result = "lower left " + part_name;
          break;
        default:
          break;
        }
      }
      break;

    case 6: // Legs/feet assume front/middle/rear, others upper/middle/lower.
      if ((location.part == BodyPart::Leg) || (location.part == BodyPart::Foot))
      {
        switch (location.number)
        {
        case 0:
          result = "front right " + part_name;
          break;
        case 1:
          result = "front left " + part_name;
          break;
        case 2:
          result = "middle right " + part_name;
          break;
        case 3:
          result = "middle left " + part_name;
          break;
        case 4:
          result = "rear right " + part_name;
          break;
        case 5:
          result = "rear left " + part_name;
          break;
        default:
          break;
        }
      }
      else
      {
        switch (location.number)
        {
        case 0:
          result = "upper right " + part_name;
          break;
        case 1:
          result = "upper left " + part_name;
          break;
        case 2:
          result = "middle right " + part_name;
          break;
        case 3:
          result = "middle left " + part_name;
          break;
        case 4:
          result = "lower right " + part_name;
          break;
        case 5:
          result = "lower left " + part_name;
          break;
        default:
          break;
        }
      }
      break;

    default:
      break;
    }

    // Anything else and we just return the ordinal name.
    /// @todo Deal with fingers/toes.
    if (result.empty())
    {
      result = Ordinal::get(location.number) + " " + part_name;
    }

    return result;
  }

  std::string Narrator::getVerb2(std::string verb) const
  {
    auto& dict = S<IStrings>();
    return dict.get("VERB_" + verb + "_2");
  }

  std::string Narrator::getVerb3(std::string verb) const
  {
    auto& dict = S<IStrings>();
    return dict.get("VERB_" + verb + "_3");
  }

  std::string Narrator::getVerbing(std::string verb) const
  {
    auto& dict = S<IStrings>();
    return dict.get("VERB_" + verb + "_GER");
  }

  std::string Narrator::getVerbed(std::string verb) const
  {
    auto& dict = S<IStrings>();
    return dict.get("VERB_" + verb + "_P2");
  }

  std::string Narrator::getVerbPP(std::string verb) const
  {
    auto& dict = S<IStrings>();
    return dict.get("VERB_" + verb + "_PP");
  }

  std::string Narrator::getVerbable(std::string verb) const
  {
    auto& dict = S<IStrings>();
    return dict.get("VERB_" + verb + "_ABLE");
  }

  void Narrator::doCycleUpdate()
  {}

  void Narrator::setMap_V(MapID newMap)
  {}

  bool Narrator::onEvent(Event const & event)
  {
    return false;
  }
} // end namespace Systems