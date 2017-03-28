#include "stdafx.h"

#include "ActionGet.h"
#include "ActionMove.h"
#include "services/IMessageLog.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

namespace Actions
{
  ActionGet ActionGet::prototype;
  ActionGet::ActionGet() : Action("get", "GET", ActionGet::create_) {}
  ActionGet::ActionGet(EntityId subject) : Action(subject, "get", "GET") {}
  ActionGet::~ActionGet() {}

  std::unordered_set<Trait> const & ActionGet::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObject,
      Trait::CanTakeAQuantity,
      Trait::ObjectMustBeMovableBySubject,
      Trait::ObjectMustNotBeInInventory
    };

    return traits;
  }

  StateResult ActionGet::doPreBeginWorkNVI(AnyMap& params)
  {
    std::string message;
    auto subject = getSubject();
    auto object = getObject();
    EntityId location = subject->getLocation();

    // Verify that the Action has an object.
    if (object == EntityId::Mu())
    {
      return StateResult::Failure();
    }

    /// @todo When picking up, check if our inventory is full-up.
    if (false)
    {
      message = maketr("YOU_TRY_TO_VERB_THE_FOO");
      Service<IMessageLog>::get().add(message);

      message = maketr("YOUR_INVENTORY_CANT_HOLD_THE_FOO");
      Service<IMessageLog>::get().add(message);
      return StateResult::Failure();
    }

    return StateResult::Success();
  }

  StateResult ActionGet::doBeginWorkNVI(AnyMap& params)
  {
    /// @todo Handle getting a certain quantity of an item.
    StateResult result = StateResult::Failure();
    std::string message;
    auto subject = getSubject();
    auto object = getObject();

    if (object->be_object_of(*this, subject) == ActionResult::Success)
    {
      message = maketr("YOU_CVERB_THE_FOO");
      Service<IMessageLog>::get().add(message);
      if (object->move_into(subject))
      {
        /// @todo Figure out action time.
        result = StateResult::Success();
      }
      else // could not add to inventory
      {
        message = maketr("YOU_CANT_VERB_FOO_UNKNOWN");
        Service<IMessageLog>::get().add(message);

        CLOG(WARNING, "Action") << "Could not move Entity " << object <<
          " even though be_object_of returned Success";
      }
    }

    return result;
  }

  StateResult ActionGet::doFinishWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionGet::doAbortWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }
} // end namespace
