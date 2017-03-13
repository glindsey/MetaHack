#include "stdafx.h"

#include "ActionShoot.h"
#include "IMessageLog.h"
#include "IStringDictionary.h"
#include "Service.h"
#include "Entity.h"
#include "EntityId.h"

namespace Actions
{
  //ACTION_SRC_BOILERPLATE(ActionShoot, "shoot", "shoot")
  //ACTION_TRAIT(can_be_subject_verb_object_preposition_direction)

  ActionShoot ActionShoot::prototype;
  ActionShoot::ActionShoot() : Action("shoot", "SHOOT", ActionShoot::create_) {}
  ActionShoot::ActionShoot(EntityId subject) : Action(subject, "shoot", "SHOOT") {}
  ActionShoot::~ActionShoot() {}

  std::unordered_set<Trait> const & ActionShoot::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObjectPrepositionDirection
    };

    return traits;
  }

  StateResult ActionShoot::do_prebegin_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionShoot::do_begin_work_(AnyMap& params)
  {
    auto& dict = Service<IStringDictionary>::get();
    Service<IMessageLog>::get().add(dict.get("ACTN_NOT_IMPLEMENTED"));

    return StateResult::Failure();
  }

  StateResult ActionShoot::do_finish_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionShoot::do_abort_work_(AnyMap& params)
  {
    return StateResult::Success();
  }
} // end namespace

