#include "stdafx.h"

#include "ActionRead.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "systems/SystemManager.h"
#include "systems/SystemNarrator.h"
#include "utilities/Shortcuts.h"

#include "entity/Entity.h" // needed for beObjectOf()

namespace Actions
{
  ActionRead ActionRead::prototype;
  ActionRead::ActionRead() : Action("READ", ActionRead::create_) {}
  ActionRead::ActionRead(EntityId subject) : Action(subject, "READ") {}
  ActionRead::~ActionRead() {}

  std::unordered_set<Trait> const & ActionRead::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObject,
      Trait::CanBeSubjectVerbDirection
    };

    return traits;
  }

  StateResult ActionRead::doPreBeginWorkNVI(GameState& gameState, SystemManager& systems, json& arguments)
  {
    auto& narrator = systems.narrator();
    auto subject = getSubject();
    auto object = getObject();

    if (false) ///< @todo Intelligence tests for reading.
    {
      printMessageTry(systems, arguments);
      putMsg(narrator.makeTr("READ_TOO_STUPID", arguments));
      return StateResult::Failure();
    }

    return StateResult::Success();
  }

  StateResult ActionRead::doBeginWorkNVI(GameState& gameState, SystemManager& systems, json& arguments)
  {
    StateResult result = StateResult::Failure();
    auto subject = getSubject();
    auto object = getObject();

    /// @todo Figure out read time.
    printMessageBegin(systems, arguments);
    result = StateResult::Success(1);

    return result;
  }

  StateResult ActionRead::doFinishWorkNVI(GameState& gameState, SystemManager& systems, json& arguments)
  {
    StateResult result = StateResult::Failure();
    auto subject = getSubject();
    auto object = getObject();

    printMessageFinish(systems, arguments);

    /// @todo Split read time into start/finish actions.
    if (object->beObjectOf(*this, subject))
    {
      /// @todo Handle destruction on read. (Technically the Lua script should be able to do this.)
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

  StateResult ActionRead::doAbortWorkNVI(GameState& gameState, SystemManager& systems, json& arguments)
  {
    auto subject = getSubject();
    auto object = getObject();

    printMessageStop(systems, arguments);
    return StateResult::Success();
  }

  void ActionRead::printMessageCant(SystemManager& systems, json& arguments) const
  {
    auto& narrator = systems.narrator();
    arguments["item"] = tr("NOUN_WRITING");
    putMsg(narrator.makeTr("THE_FOO_HAS_NO_NOUN_TO_VERB", arguments));
  }
} // end namespace
