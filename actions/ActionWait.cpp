#include "stdafx.h"

#include "ActionWait.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "services/Service.h"
#include "systems/Manager.h"
#include "systems/SystemNarrator.h"
#include "utilities/Shortcuts.h"

namespace Actions
{
  ActionWait ActionWait::prototype;
  ActionWait::ActionWait() : Action("WAIT", ActionWait::create_) {}
  ActionWait::ActionWait(EntityId subject) : Action(subject, "WAIT") {}
  ActionWait::~ActionWait() {}

  ReasonBool ActionWait::subjectIsCapable(GameState const& gameState) const
  {
    // Any entity can wait, whenever.
    return { true, "" };
  }

  std::unordered_set<Trait> const & ActionWait::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectOnly
    };

    return traits;
  }

  StateResult ActionWait::doPreBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    // We can always wait.
    return StateResult::Success();
  }

  StateResult ActionWait::doBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    auto& narrator = systems.narrator();

    /// @todo Handle a variable amount of time.
    arguments["time"] = std::to_string(1);
    putMsg(narrator.makeTr("YOU_VERB_FOR_X_TIME", arguments));

    return{ true, 1 };
  }

  StateResult ActionWait::doFinishWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  StateResult ActionWait::doAbortWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }
} // end namespace

