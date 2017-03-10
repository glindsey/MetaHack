#include "stdafx.h"

#include "ActionGet.h"
#include "ActionMove.h"
#include "IMessageLog.h"
#include "Service.h"
#include "Entity.h"
#include "EntityId.h"

namespace Actions
{
  ActionGet ActionGet::prototype;
  ActionGet::ActionGet() : Action("get", "GET", ActionGet::create_) {}
  ActionGet::ActionGet(EntityId subject) : Action(subject, "get", "GET") {}
  ActionGet::~ActionGet() {}

  std::unordered_set<Action::Trait> const & ActionGet::getTraits() const
  {
    static std::unordered_set<Action::Trait> traits =
    {
      Trait::CanBeSubjectVerbObject,
      Trait::CanTakeAQuantity
    };

    return traits;
  }

  StateResult ActionGet::do_prebegin_work_(AnyMap& params)
  {
    std::string message;
    auto subject = get_subject();
    auto object = get_object();
    EntityId location = subject->getLocation();

    // Verify that the Action has an object.
    if (object == EntityId::Mu())
    {
      return StateResult::Failure();
    }

    // Check that it isn't US!
    if (object == subject)
    {
      if (IS_PLAYER)
      {
        message = "Oh, ha ha, I get it, \"pick me up\".  Nice try.";
      }
      else
      {
        message = YOU_TRY + " to pick " + YOURSELF +
          "up, which seriously shouldn't happen.";
        CLOG(WARNING, "Action") << "NPC tried to pick self up!?";
      }
      Service<IMessageLog>::get().add(message);
      return StateResult::Failure();
    }

    // Check if it's already in our inventory.
    if (subject->get_inventory().contains(object))
    {
      message = YOU_TRY + " to pick up " + THE_FOO + ".";
      Service<IMessageLog>::get().add(message);

      message = THE_FOO + FOO_IS + " already in " + YOUR + " inventory!";
      Service<IMessageLog>::get().add(message);
      return StateResult::Failure();
    }

    /// @todo When picking up, check if our inventory is full-up.
    if (false)
    {
      message = YOU_TRY + " to pick up " + THE_FOO + ".";
      Service<IMessageLog>::get().add(message);

      message = YOUR + " inventory cannot accommodate " + THE_FOO + ".";
      Service<IMessageLog>::get().add(message);
      return StateResult::Failure();
    }

    if (!object->can_have_action_done_by(subject, ActionMove::prototype))
    {
      message = YOU_TRY + " to pick up " + THE_FOO + ".";
      Service<IMessageLog>::get().add(message);

      message = YOU + " cannot move " + THE_FOO + ".";
      Service<IMessageLog>::get().add(message);
      return StateResult::Failure();
    }

    return StateResult::Success();
  }

  StateResult ActionGet::do_begin_work_(AnyMap& params)
  {
    /// @todo Handle getting a certain quantity of an item.
    StateResult result = StateResult::Failure();
    std::string message;
    auto subject = get_subject();
    auto object = get_object();

    if (object->be_object_of(*this, subject) == ActionResult::Success)
    {
      message = YOU + CV(" pick", " picks") + " up " + THE_FOO + ".";
      Service<IMessageLog>::get().add(message);
      if (object->move_into(subject))
      {
        /// @todo Figure out action time.
        result = StateResult::Success();
      }
      else // could not add to inventory
      {
        message = YOU + " could not pick up " + THE_FOO + " for some inexplicable reason.";
        Service<IMessageLog>::get().add(message);

        CLOG(WARNING, "Action") << "Could not move Entity " << object <<
          " even though be_object_of returned Success";
      }
    }

    return result;
  }

  StateResult ActionGet::do_finish_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionGet::do_abort_work_(AnyMap& params)
  {
    return StateResult::Success();
  }
} // end namespace
