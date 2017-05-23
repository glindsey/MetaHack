#include "stdafx.h"

#include "ActionClose.h"
#include "components/ComponentManager.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

namespace Actions
{
  ActionClose ActionClose::prototype;
  ActionClose::ActionClose() : Action("close", "CLOSE", ActionClose::create_) {}
  ActionClose::ActionClose(EntityId subject) : Action(subject, "close", "CLOSE") {}
  ActionClose::~ActionClose() {}

  ReasonBool ActionClose::subjectIsCapable(GameState const& gameState) const
  {
    auto subject = getSubject();
    bool isSapient = COMPONENTS.sapience.existsFor(subject);
    bool canGrasp = COMPONENTS.bodyparts.existsFor(subject) && COMPONENTS.bodyparts[subject].hasPrehensileBodyPart();

    if (!isSapient) return { false, "YOU_ARE_NOT_SAPIENT" }; ///< @todo Add translation key
    if (!canGrasp) return { false, "YOU_HAVE_NO_GRASPING_BODYPARTS" }; ///< @todo Add translation key

    return { true, "" };
  }

  std::unordered_set<Trait> const & ActionClose::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObject,
      Trait::CanBeSubjectVerbDirection
    };

    return traits;
  }

  StateResult ActionClose::doPreBeginWorkNVI(GameState& gameState)
  {
    return StateResult::Success();
  }

  StateResult ActionClose::doBeginWorkNVI(GameState& gameState)
  {
    bool success = false;
    unsigned int action_time = 0;

    putTr("ACTN_NOT_IMPLEMENTED");

#if 0
    if (entity != EntityId::Mu())
    {
      success = actor->do_close(entity, action_time);
    }
#endif

    return{ success, action_time };
  }

  StateResult ActionClose::doFinishWorkNVI(GameState& gameState)
  {
    return StateResult::Success();
  }

  StateResult ActionClose::doAbortWorkNVI(GameState& gameState)
  {
    return StateResult::Success();
  }
}

