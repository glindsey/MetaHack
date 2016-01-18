#include "ActionPutInto.h"
#include "Thing.h"
#include "ThingRef.h"

ActionPutInto::ActionPutInto()
  :
  Action()
{}

ActionPutInto::~ActionPutInto()
{}

bool ActionPutInto::target_can_be_thing() const
{
  return true;
}

bool ActionPutInto::target_can_be_direction() const
{
  return false;
}

Action::StateResult ActionPutInto::do_prebegin_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionPutInto::do_begin_work(ThingRef actor, ThingRef thing, AnyMap& params)
{
  bool success = false;
  unsigned int action_time;

  ThingRef container = get_target_thing();
  if (container != ThingManager::get_mu())
  {
    if (container->get_intrinsic<int>("inventory_size") != 0)
    {
      if (thing != ThingManager::get_mu())
      {
        success = actor->do_put_into(thing, container, action_time);
      }
    }
  }
  else
  {
    the_message_log.add("That target is not a container.");
  }

  return{ success, action_time };
}

Action::StateResult ActionPutInto::do_finish_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionPutInto::do_abort_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}