#pragma once

#include "components/ComponentGender.h"
#include "components/ComponentGlobals.h"
#include "components/ComponentHealth.h"
#include "components/ComponentMap.h"
#include "components/ComponentPosition.h"
#include "systems/SystemCRTP.h"
#include "types/Gender.h"

// Forward declarations
class EntityId;

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

/// System that deals with creating strings based on the game state.
class SystemNarrator : public SystemCRTP<SystemNarrator>
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

  SystemNarrator(ComponentGlobals const& globals,
                 ComponentMap<std::string> const& category,
                 ComponentMap<ComponentGender> const& gender,
                 ComponentMap<ComponentHealth> const& health,
                 ComponentMap<ComponentPosition> const& position,
                 ComponentMap<std::string> const& properName,
                 ComponentMap<unsigned int> const& quantity);
  virtual ~SystemNarrator();

  /// Return true if a third-person verb form should be used.
  /// This function checks to see if this Entity is currently designated as
  /// the player, or has a quantity greater than zero.
  /// If so, it returns true; otherwise, it returns false.
  /// @todo See if this is different for languages other than English.
  ///       I'm guessing the answer is "yes, yes it is".
  bool isThirdPerson(EntityId id) const;

  /// Choose which verb form to use based on first/second/third person.
  /// This function checks to see if this Entity is currently designated as
  /// the player, or has a quantity greater than zero.
  /// If so, it returns the string passed as verb2; otherwise, it returns the
  /// string passed as verb3.
  /// @param verb2 The second person or plural verb form, such as "shake"
  /// @param verb3 The third person verb form, such as "shakes"
  std::string const & chooseVerb(EntityId id, 
                                 std::string const & verb12, 
                                 std::string const & verb3) const;

  /// @addtogroup Pronouns
  /// @todo Make localizable. (How? Use Lua scripts maybe?)
  /// @{

  /// Get the appropriate subject pronoun for an Entity.
  std::string const& getSubjectPronoun(EntityId id) const;

  /// Get the appropriate object pronoun for an Entity.
  std::string const& getObjectPronoun(EntityId id) const;

  /// Get the appropriate reflexive pronoun for an Entity.
  std::string const& getReflexivePronoun(EntityId id) const;

  /// Get the appropriate possessive adjective for an Entity.
  std::string const& getPossessiveAdjective(EntityId id) const;

  /// Get the appropriate possessive pronoun for an Entity.
  std::string const& getPossessivePronoun(EntityId id) const;

  /// @}

  /// Return either the gender of an Entity, Gender::You if the Entity 
  /// is the player, or Gender::Plural if there's more than one in an
  /// aggregate Entity.
  Gender getGenderOrYou(EntityId) const;

  /// Return a string that identifies an object, in the subjective case.
  /// If it IS the player, it'll return "you".
  /// Otherwise it calls getDescriptiveString().
  ///
  /// @param articles Choose whether to use definite or indefinite articles.
  ///                 Defaults to definite articles.
  std::string getSubjectiveString(EntityId id,
                                  ArticleChoice articles = ArticleChoice::Definite) const;

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
                                 ArticleChoice articles = ArticleChoice::Definite) const;

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
                                 ArticleChoice articles = ArticleChoice::Definite) const;

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
  std::string getPossessiveString(EntityId id, std::string owned, std::string adjectives = "") const;

  /// Return an entity's adjective qualifiers (such as "fireproof", "waterproof", etc.)
  std::string getDisplayAdjectives(EntityId id) const;

  /// Get a const reference to an entity's category data.
  json const& getCategoryData(EntityId id) const;

  /// Return an entity's name.
  std::string getDisplayName(EntityId id) const;

  /// Return an entity's plural.
  std::string getDisplayPlural(EntityId id) const;

  // String composition functions
  std::string makeString(EntityId subject, EntityId object, std::string pattern, std::vector<std::string> optionalStrings) const;
  std::string makeString(EntityId subject, EntityId object, std::string pattern) const;
  std::string makeStringNumsOnly(std::string pattern, std::vector<std::string> optionalStrings) const;
  std::string makeTr(EntityId subject, EntityId object, std::string key) const;
  std::string makeTr(EntityId subject, EntityId object, std::string key, std::vector<std::string> optionalStrings) const;

  std::string replaceTokens(std::string str,
                            std::function<std::string(std::string)> tokenFunctor,
                            std::function<std::string(std::string, std::string)> tokenArgumentFunctor,
                            std::function<bool(std::string)> chooseFunctor) const;

  /// Recalculate whatever needs recalculating.
  void doCycleUpdate();

protected:
  void setMapNVO(MapID newMap);

  virtual bool onEvent(Event const& event) override;

private:
  // Components used by this system.
  ComponentGlobals const& m_globals;
  ComponentMap<std::string> const& m_category;
  ComponentMap<ComponentGender> const& m_gender;
  ComponentMap<ComponentHealth> const& m_health;
  ComponentMap<ComponentPosition> const& m_position;
  ComponentMap<std::string> const& m_properName;
  ComponentMap<unsigned int> const& m_quantity;
};