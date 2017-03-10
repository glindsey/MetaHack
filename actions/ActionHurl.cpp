#include "stdafx.h"

#include "ActionHurl.h"
#include "IMessageLog.h"
#include "Service.h"
#include "Entity.h"
#include "EntityId.h"


namespace Actions
{
  ActionHurl ActionHurl::prototype;
  ActionHurl::ActionHurl() : Action("hurl", "THROW", ActionHurl::create_) {}
  ActionHurl::ActionHurl(EntityId subject) : Action(subject, "hurl", "THROW") {}
  ActionHurl::~ActionHurl() {}

  std::unordered_set<Action::Trait> const & ActionHurl::getTraits() const
  {
    static std::unordered_set<Action::Trait> traits =
    {
      Trait::CanBeSubjectVerbObjectPrepositionDirection
    };

    return traits;
  }

  StateResult ActionHurl::do_prebegin_work_(AnyMap& params)
  {
    std::string message;
    auto subject = get_subject();
    auto object = get_object();
    auto new_direction = get_target_direction();

    // Check that it isn't US!
    if (object == subject)
    {
      if (IS_PLAYER)
      {
        message = "Throw yourself?  Throw yourself what, a party?";
      }
      else
      {
        message = YOU_TRY + " to throw " + YOURSELF +
          ", which seriously shouldn't happen.";
        CLOG(WARNING, "Action") << "NPC tried to throw self!?";
      }
      Service<IMessageLog>::get().add(message);

      return StateResult::Failure();
    }

    // Check that it's in our inventory.
    if (!subject->get_inventory().contains(object))
    {
      message = YOU_TRY + " to throw " + THE_FOO + ".";
      Service<IMessageLog>::get().add(message);

      message = "But " + THE_FOO + FOO_IS +
        " not actually in " + YOUR +
        " inventory!";
      Service<IMessageLog>::get().add(message);

      return StateResult::Failure();
    }

    /// Check that we have limbs capable of throwing.
    if (subject->get_modified_property<bool>("can_throw"))
    {
      print_message_try_();

      message = "But, as " + getIndefArt(subject->get_display_name()) + subject->get_display_name() + "," + YOU_ARE + " not capable of throwing anything.";
      Service<IMessageLog>::get().add(message);

      return StateResult::Failure();
    }

    /// Check that we're not wearing the item.
    if (subject->has_equipped(object))
    {
      print_message_try_();

      message = YOU + " cannot throw something " + YOU_ARE + "wearing.";
      Service<IMessageLog>::get().add(message);

      return StateResult::Failure();
    }

    return StateResult::Success();
  }

  StateResult ActionHurl::do_begin_work_(AnyMap& params)
  {
    auto result = StateResult::Failure();
    std::string message;
    auto subject = get_subject();
    auto object = get_object();
    auto direction = get_target_direction();
    EntityId new_location = subject->getLocation();

    if (object->be_object_of(*this, subject, direction) == ActionResult::Success)
    {
      if (object->move_into(new_location))
      {
        print_message_do_();

        /// @todo When throwing, set Entity's direction and velocity
        /// @todo Figure out action time.
        result = StateResult::Success();
      }
      else
      {
        message = YOU + " could not throw " + THE_FOO + " for some inexplicable reason.";
        Service<IMessageLog>::get().add(message);

        CLOG(WARNING, "Action") << "Could not throw Entity " << object <<
          " even though be_object_of returned Success";
      }
    }

    return result;
  }

  StateResult ActionHurl::do_finish_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionHurl::do_abort_work_(AnyMap& params)
  {
    return StateResult::Success();
  }
} // end namespace
