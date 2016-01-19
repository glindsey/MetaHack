#include "ActionQuaff.h"
#include "Thing.h"
#include "ThingRef.h"

ActionQuaff::ActionQuaff(ThingRef subject)
  :
  Action(subject)
{}

ActionQuaff::ActionQuaff(ThingRef subject, ThingRef object)
  :
  Action(subject, object)
{}

ActionQuaff::~ActionQuaff()
{}

Action::StateResult ActionQuaff::do_prebegin_work(AnyMap& params)
{
  std::string message;

  // Check that it isn't US!
  if (get_subject() == get_objects()[0])
  {
    message = YOU_TRY + " to drink " + YOURSELF + ".";
    the_message_log.add(message);

    /// @todo When drinking self, special message if caller is a liquid-based organism.
    message = "Needless to say, " + YOU_ARE + " not very successful in this endeavor.";
    the_message_log.add(message);

    return{ false, 0 };
  }

  // Check that we're capable of drinking at all.
  if (get_subject()->get_intrinsic<bool>("can_drink"))
  {
    message = YOU_TRY_TO("drink from") + FOO + ".";
    the_message_log.add(message);
    message = "But, as a " + get_subject()->get_display_name() + "," + YOU_ARE + " not capable of drinking liquids.";
    the_message_log.add(message);

    return{ false, 0 };
  }

  // Check that the thing is within reach.
  if (!get_subject()->can_reach(get_object()))
  {
    message = YOU_TRY_TO("drink from") + FOO + ", but it is out of " + YOUR + " reach.";
    the_message_log.add(message);

    return{ false, 0 };
  }

  // Check that it is something that contains a liquid.
  if (!get_object()->get_intrinsic<bool>("liquid_carrier"))
  {
    message = YOU_TRY_TO("drink from") + FOO + ".";
    the_message_log.add(message);
    message = YOU + " cannot drink from that!";
    the_message_log.add(message);

    return{ false, 0 };
  }

  // Check that it is not empty.
  Inventory& inv = get_object()->get_inventory();
  if (inv.count() == 0)
  {
    message = YOU_TRY_TO("drink from") + FOO + ".";
    the_message_log.add(message);
    message = "But " + FOO + " is empty!";
    the_message_log.add(message);

    return{ false, 0 };
  }

  return{ true, 0 };
}

Action::StateResult ActionQuaff::do_begin_work(AnyMap& params)
{
  std::string message;

  bool success = false;
  unsigned int action_time = 0;

  if (get_object()->is_drinkable_by(get_subject()))
  {
    message = YOU + " drink from " + FOO + ".";
    the_message_log.add(message);

    ActionResult result = get_object()->perform_action_drank_by(get_subject());

    switch (result)
    {
      case ActionResult::Success:
        return{ true, 1 };

      case ActionResult::SuccessDestroyed:
        get_object()->get_inventory().get(INVSLOT_ZERO)->destroy();
        return{ true, 1 };

      case ActionResult::Failure:
        message = YOU + " stop drinking.";
        the_message_log.add(message);
        return{ false, 0 };

      default:
        MINOR_ERROR("Unknown ActionResult %d", result);
        return{ false, 0 };
    }
  } // end if (thing->is_drinkable_by(pImpl->ref))
  else
  {
    message = YOU + " can't drink that!";
    the_message_log.add(message);
    return{ false, 0 };
  }
}

Action::StateResult ActionQuaff::do_finish_work(AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionQuaff::do_abort_work(AnyMap& params)
{
  return{ true, 0 };
}