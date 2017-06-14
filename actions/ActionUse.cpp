#include "stdafx.h"

#include "ActionUse.h"
#include "components/ComponentManager.h"
#include "game/GameState.h"
#include "lua/LuaObject.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "services/Service.h"
#include "systems/Manager.h"
#include "systems/SystemNarrator.h"
#include "utilities/Shortcuts.h"

#include "entity/Entity.h" // needed for beObjectOf()
namespace Actions
{
  ActionUse ActionUse::prototype;
  ActionUse::ActionUse() : Action("USE", ActionUse::create_) {}
  ActionUse::ActionUse(EntityId subject) : Action(subject, "USE") {}
  ActionUse::~ActionUse() {}

  ReasonBool ActionUse::subjectIsCapable(GameState const& gameState) const
  {
    auto subject = getSubject();
    auto& components = gameState.components();
    bool isSapient = components.sapience.existsFor(subject);
    bool canGrasp = components.bodyparts.existsFor(subject) && components.bodyparts.of(subject).hasPrehensileBodyPart();

    if (!isSapient) return { false, "YOU_ARE_NOT_SAPIENT" }; ///< @todo Add translation key
    if (!canGrasp) return { false, "YOU_HAVE_NO_GRASPING_BODYPARTS" }; ///< @todo Add translation key

    return { true, "" };
  }

  ReasonBool ActionUse::objectIsAllowed(GameState const& gameState) const
  {
    auto& object = getObject();
    auto useFunctionExists = !(gameState.lua().find_lua_function(COMPONENTS.category[object], "on_object_of_" + getType())).empty();
    return { useFunctionExists, "" };
  }


  std::unordered_set<Trait> const & ActionUse::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObject
    };

    return traits;
  }

  StateResult ActionUse::doPreBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    // All checks done in Action class via traits.
    return StateResult::Success();
  }

  StateResult ActionUse::doBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    StateResult result = StateResult::Failure();

    /// @todo Figure out use time.
    printMessageBegin(systems, arguments);
    result = StateResult::Success(1);

    return result;
  }

  StateResult ActionUse::doFinishWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    StateResult result = StateResult::Failure();
    auto subject = getSubject();
    auto object = getObject();

    printMessageFinish(systems, arguments);

    /// @todo Split use time into start/finish actions.
    if (object->beObjectOf(*this, subject))
    {
      /// @todo Handle object destruction on use. (Lua script can do this.)
      //putTr("THE_FOO_DISINTEGRATES_AFTER_YOU_VERB");
      //systems.janitor().markForDeletion(object);
      result = StateResult::Success();
    }
    else
    {
      result = StateResult::Failure();
    }

    return result;
  }

  StateResult ActionUse::doAbortWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    printMessageStop(systems, arguments);
    return StateResult::Success();
  }
} // end namespace
