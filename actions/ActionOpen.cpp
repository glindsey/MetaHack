#include "stdafx.h"

#include "ActionOpen.h"
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

  std::unordered_set<Trait> const & ActionOpen::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObject,
      Trait::CanBeSubjectVerbDirection
    };

    return traits;
  }

  StateResult ActionOpen::doPreBeginWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionOpen::doBeginWorkNVI(AnyMap& params)
  {
    bool success = false;
    unsigned int action_time = 0;

    auto& dict = Service<IStringDictionary>::get();
    Service<IMessageLog>::get().add(dict.get("ACTN_NOT_IMPLEMENTED"));

#if 0
    if (entity != EntityId::Mu())
    {
      success = actor->do_open(entity, action_time);
    }
#endif

    return{ success, action_time };
  }

  StateResult ActionOpen::doFinishWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionOpen::doAbortWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }
} // end namespace

