#include "stdafx.h"

#include "ActionClose.h"
#include "IMessageLog.h"
#include "IStringDictionary.h"
#include "Service.h"
#include "Entity.h"
#include "EntityId.h"

namespace Actions
{
  ActionClose ActionClose::prototype;
  ActionClose::ActionClose() : Action("close", "CLOSE", ActionClose::create_) {}
  ActionClose::ActionClose(EntityId subject) : Action(subject, "close", "CLOSE") {}
  ActionClose::~ActionClose() {}

  std::unordered_set<Trait> const & ActionClose::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObject,
      Trait::CanBeSubjectVerbDirection
    };

    return traits;
  }

  StateResult ActionClose::do_prebegin_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionClose::do_begin_work_(AnyMap& params)
  {
    bool success = false;
    unsigned int action_time = 0;

    auto& dict = Service<IStringDictionary>::get();
    Service<IMessageLog>::get().add(dict.get("ACTN_NOT_IMPLEMENTED"));

#if 0
    if (entity != EntityId::Mu())
    {
      success = actor->do_close(entity, action_time);
    }
#endif

    return{ success, action_time };
  }

  StateResult ActionClose::do_finish_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionClose::do_abort_work_(AnyMap& params)
  {
    return StateResult::Success();
  }
}

