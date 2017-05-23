#include "stdafx.h"

#include "ActionUse.h"
#include "components/ComponentManager.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

namespace Actions
{
  ActionUse ActionUse::prototype;
  ActionUse::ActionUse() : Action("use", "USE", ActionUse::create_) {}
  ActionUse::ActionUse(EntityId subject) : Action(subject, "use", "USE") {}
  ActionUse::~ActionUse() {}

  ReasonBool ActionUse::subjectIsCapable(GameState const& gameState) const
  {
    auto subject = getSubject();
    bool isSapient = COMPONENTS.sapience.existsFor(subject);
    bool canGrasp = COMPONENTS.bodyparts.existsFor(subject) && COMPONENTS.bodyparts[subject].hasPrehensileBodyPart();

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

  StateResult ActionUse::doPreBeginWorkNVI(GameState& gameState)
  {
    // All checks done in Action class via traits.
    return StateResult::Success();
  }

  StateResult ActionUse::doBeginWorkNVI(GameState& gameState)
  {
    StateResult result = StateResult::Failure();
    std::string message;
    auto subject = getSubject();
    auto object = getObject();

    /// @todo Figure out use time.
    printMessageBegin();
    result = StateResult::Success(1);

    return result;
  }

  StateResult ActionUse::doFinishWorkNVI(GameState& gameState)
  {
    StateResult result = StateResult::Failure();
    std::string message;
    auto subject = getSubject();
    auto object = getObject();

    printMessageFinish();

    /// @todo Split read time into start/finish actions.
    if (object->beObjectOf(*this, subject))
    {
      /// @todo Handle object destruction on use. (Lua script can do this.)
      //putTr("THE_FOO_DISINTEGRATES_AFTER_YOU_VERB");
      //object->destroy();
      result = StateResult::Success();
    }
    else
    {
      result = StateResult::Failure();
    }

    return result;
  }

  StateResult ActionUse::doAbortWorkNVI(GameState& gameState)
  {
    auto subject = getSubject();
    auto object = getObject();

    printMessageStop();

    return StateResult::Success();
  }
} // end namespace
