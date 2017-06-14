#pragma once

#include "systems/CRTP.h"
#include "types/Bodypart.h"
#include "types/Direction.h"
#include "types/Gender.h"

#include "json.hpp"
using json = ::nlohmann::json;

// Forward declarations
class EntityId;
namespace Components
{
  class ComponentManager;
}

using EntitySet = std::set<EntityId>;

// Associated enum classes to aid in parameter legibility.
enum class ArticleChoice
{
  Indefinite,
  Definite
};

enum class UsePossessives
{
  No,
  Yes
};

namespace Systems
{

  /// System that deals with creating strings based on the game state.
  class Narrator : public CRTP<Narrator>
  {
  public:
    /// Enum used for the replaceTokens state machine.
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

    friend std::ostream& operator<<(std::ostream& os, TokenizerState state);

    Narrator(Components::ComponentManager const& components);
    virtual ~Narrator();

    /// Return true if a third-person verb form should be used.
    /// This function checks to see if this Entity is currently designated as
    /// the player, or has a quantity greater than zero.
    /// If so, it returns true; otherwise, it returns false.
    /// @todo See if this is different for languages other than English.
    ///       I'm guessing the answer is "yes, yes it is".
    bool isThirdPerson(EntitySet entities) const;

    /// Choose which verb form to use based on first/second/third person.
    /// This function checks to see if this Entity is currently designated as
    /// the player, or has a quantity greater than zero.
    /// If so, it returns the string passed as verb2; otherwise, it returns the
    /// string passed as verb3.
    /// @param verb2 The second person or plural verb form, such as "shake"
    /// @param verb3 The third person verb form, such as "shakes"
    std::string const & chooseVerb(EntitySet entities,
                                   std::string const & verb12,
                                   std::string const & verb3) const;

    /// @addtogroup Pronouns
    /// @todo Make localizable. (How? Use Lua scripts maybe?)
    /// @{

    /// Get the appropriate subject pronoun for one or more Entities.
    std::string const& getSubjectPronoun(EntitySet entities) const;

    /// Get the appropriate object pronoun for one or more Entities.
    std::string const& getObjectPronoun(EntitySet entities) const;

    /// Get the appropriate reflexive pronoun for one or more Entities.
    std::string const& getReflexivePronoun(EntitySet entities) const;

    /// Get the appropriate possessive adjective for one or more Entities.
    std::string const& getPossessiveAdjective(EntitySet entities) const;

    /// Get the appropriate possessive pronoun for one or more Entities.
    std::string const& getPossessivePronoun(EntitySet entities) const;

    /// @}

    /// Return either the gender of an Entity, Gender::You if the Entity 
    /// is the player, or Gender::Plural if there's more than one in an
    /// aggregate Entity.
    Gender getGenderOrYou(EntitySet entities) const;

    /// Return a string that identifies an object, in the subjective case.
    /// If it IS the player, it'll return "you".
    /// Otherwise it calls getDescriptiveString().
    ///
    /// @param articles Choose whether to use definite or indefinite articles.
    ///                 Defaults to definite articles.
    std::string getSubjectiveString(EntityId id,
                                    ArticleChoice articles = ArticleChoice::Definite,
                                    UsePossessives possessives = UsePossessives::Yes) const;

    /// Return a string that identifies an entity, in the objective case.
    /// If it IS the player, it'll return "you".
    /// Otherwise it calls getDescriptiveString().
    ///
    /// @param articles Choose whether to use definite or indefinite articles.
    ///                 Defaults to definite articles.
    ///
    /// @note In English this has the same results as 
    /// getSubjectiveString(), but _this will not be the case
    /// in all languages_.
    std::string getObjectiveString(EntityId id,
                                   ArticleChoice articles = ArticleChoice::Definite,
                                   UsePossessives possessives = UsePossessives::Yes) const;

    /// Return a string that identifies an entity, in the reflexive case.
    /// If it matches the object passed in as "other", it'll return
    /// the appropriate reflexive pronoun ("yourself", "itself", etc.).
    /// Otherwise it calls getDescriptiveString().
    ///
    /// @param other      The "other" to compare to.
    /// @param articles Choose whether to use definite or indefinite articles.
    ///                 Defaults to definite articles.
    std::string getReflexiveString(EntityId id,
                                   EntityId other,
                                   ArticleChoice articles = ArticleChoice::Definite,
                                   UsePossessives possessives = UsePossessives::Yes) const;

    std::string getDescription(EntitySet entities,
                               std::function<std::string(EntityId)> descriptionFunctor) const;

    /// Return a string that identifies an entity.
    /// Returns "the/a/an" and a description of the entity, such as
    /// "the chair".
    /// If it is carried by the player, and possessives = true, it'll
    /// return "your (entity)".
    /// Likewise, if it is carried by another DynamicEntity it'll return
    /// "(DynamicEntity)'s (entity)".
    /// @todo Make localizable. (How? Use Lua scripts maybe?)
    ///
    /// @param articles Choose whether to use definite or indefinite articles.
    ///                 Defaults to definite articles.
    /// @param possessives  Choose whether to use possessive articles when appropriate.
    ///                     Defaults to using them.
    std::string getDescriptiveString(EntityId id,
                                     ArticleChoice articles = ArticleChoice::Definite,
                                     UsePossessives possessives = UsePossessives::Yes) const;

    /// Choose the proper possessive form for a string passed in.
    /// For a Entity, this is simply "the foo", as Entities cannot own entities.
    /// This function checks to see if this Entity is currently designated as
    /// the player.  If so, it returns "your foo".  If not, it returns getName() + "'s foo".
    /// @todo Make localizable. (How? Use Lua scripts maybe?)
    ///
    /// @param owned      String name of the thing that is possessed.
    /// @param adjectives Optional adjectives to add.
    ///
    /// @note If you want a possessive pronoun like his/her/its/etc., use
    /// getPossessiveAdjective().
    std::string getPossessiveString(EntityId id,
                                    std::string owned,
                                    std::string adjectives = "") const;

    /// Get a const reference to an entity's category data.
    json const& getCategoryData(EntityId id) const;

    /// Return an entity's adjective qualifiers (such as "fireproof", "waterproof", etc.)
    std::string getDisplayAdjectives(EntityId id) const;

    /// Return an entity's name.
    std::string getDisplayName(EntityId id) const;

    /// Return an entity's plural.
    std::string getDisplayPlural(EntityId id) const;

    // String composition functions
    std::string makeString(std::string pattern,
                           json arguments = json::object()) const;

    std::string makeStringTokensOnly(std::string pattern,
                                     json arguments = json::object()) const;

    std::string makeTr(std::string key,
                       json arguments = json::object()) const;

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
    std::string replaceTokens(std::string str,
                              std::function<std::string(std::string)> tokenFunctor,
                              std::function<std::string(std::string, std::string)> tokenArgumentFunctor,
                              std::function<bool(std::string)> chooseFunctor) const;

    /// Get the number of a particular body part an Entity has.
    unsigned int getBodypartNumber(EntityId id, BodyPart part) const;

    /// Get the appropriate body part name for an Entity.
    std::string getBodypartName(EntityId id, BodyPart part) const;

    /// Get the appropriate body part plural for an Entity.
    std::string getBodypartPlural(EntityId id, BodyPart part) const;

    /// Get the appropriate description for an Entity's body part.
    /// This takes the body part name and the number referencing the particular
    /// part and comes up with a description.
    /// For example, for most creatures with two hands, hand #0 will be the
    /// "right hand" and hand #1 will be the "left hand".
    /// In most cases the default implementation here will work, but if a
    /// creature has (for example) a strange configuration of limbs this can be
    /// overridden.
    std::string getBodypartDescription(EntityId id, BodyLocation location);

    /// Return the first-/second-person singular form of a verb.
    /// @todo English doesn't generally distinguish between 1st/2nd person
    ///       for past tense, but some other languages do. Assuming, of course,
    ///       we even NEED the 1st person conjugation.
    std::string getVerb2(std::string verb) const;

    /// Return the third-person singular form of a verb.
    std::string getVerb3(std::string verb) const;

    /// Return the present participle form of a verb.
    std::string getVerbing(std::string verb) const;

    /// Return the past form of a verb.
    /// @todo English doesn't generally distinguish between 2nd/3rd person
    ///       for past tense, but some other languages do.
    std::string getVerbed(std::string verb) const;

    /// Return the past participle form of a verb.
    std::string getVerbPP(std::string verb) const;

    /// Return the adjective form of the verb to be performed.
    /// (Obviously, some verbs don't make sense when conjugated in this way, 
    /// but the option is there nonetheless.)
    std::string getVerbable(std::string verb) const;

    /// Recalculate whatever needs recalculating.
    void doCycleUpdate();

  protected:
    virtual void setMap_V(MapID newMap) override;

    virtual bool onEvent(Event const& event) override;

  private:
    /// Components used by this system.
    /// Narrator touches enough components that we just pass a reference to
    /// the entire manager in, instead of individual component maps.
    Components::ComponentManager const& m_components;
  };

} // end namespace Systems
