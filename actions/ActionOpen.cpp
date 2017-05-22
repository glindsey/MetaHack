#include "stdafx.h"

#include "ActionOpen.h"
#include "components/ComponentManager.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

namespace Actions
{
  ActionOpen ActionOpen::prototype;
  ActionOpen::ActionOpen() : Action("open", "OPEN", ActionOpen::create_) {}
  ActionOpen::ActionOpen(EntityId subject) : Action(subject, "open", "OPEN") {}
  ActionOpen::~ActionOpen() {}

  ReasonBool ActionOpen::subjectIsCapable(GameState& gameState) const
  {
    auto subject = getSubject();
    bool isSapient = COMPONENTS.sapience.existsFor(subject);
    bool canGrasp = COMPONENTS.bodyparts.existsFor(subject) && COMPONENTS.bodyparts[subject].hasPrehensileBodyPart();

    if (!isSapient) return { false, "YOU_ARE_NOT_SAPIENT" }; ///< @todo Add translation key
    if (!canGrasp) return { false, "YOU_HAVE_NO_GRASPING_BODYPARTS" }; ///< @todo Add translation key

    return { true, "" };
  }

  std::unordered_set<Trait> const & ActionOpen::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObject,
      Trait::CanBeSubjectVerbDirection
    };

    return traits;
  }

  StateResult ActionOpen::doPreBeginWorkNVI(GameState& gameState, AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionOpen::doBeginWorkNVI(GameState& gameState, AnyMap& params)
  {
    bool success = false;
    unsigned int action_time = 0;
    putTr("ACTN_NOT_IMPLEMENTED");

#if 0
    if (entity != EntityId::Mu())
    {
      success = actor->do_open(entity, action_time);
    }
#endif

    return{ success, action_time };
  }

  StateResult ActionOpen::doFinishWorkNVI(GameState& gameState, AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionOpen::doAbortWorkNVI(GameState& gameState, AnyMap& params)
  {
    return StateResult::Success();
  }
} // end namespace

