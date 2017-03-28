#include "stdafx.h"

#include "ActionShoot.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

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

  StateResult ActionShoot::doPreBeginWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionShoot::doBeginWorkNVI(AnyMap& params)
  {
    putTr("ACTN_NOT_IMPLEMENTED");
    return StateResult::Failure();
  }

  StateResult ActionShoot::doFinishWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionShoot::doAbortWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }
} // end namespace

