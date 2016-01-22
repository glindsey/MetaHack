#include "ActionPutInto.h"
#include "Thing.h"
#include "ThingRef.h"

ActionPutInto::ActionPutInto(ThingRef subject, ThingRef object)
  :
  Action(subject, object)
{}

ActionPutInto::ActionPutInto(ThingRef subject, std::vector<ThingRef> objects)
  :
  Action(subject, objects)
{}

ActionPutInto::~ActionPutInto()
{}

Action::StateResult ActionPutInto::do_prebegin_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionPutInto::do_begin_work(AnyMap& params)
{
  bool success = false;
  unsigned int action_time;

  ThingRef thing = get_objects().front();
  ThingRef container = get_target_thing();
  if (container != ThingManager::get_mu())
  {
    if (container->get_intrinsic<int>("inventory_size") != 0)
    {
      if (thing != ThingManager::get_mu())
      {
        success = get_subject()->do_put_into(thing, container, action_time);
      }
    }
  }
  else
  {
    the_message_log.add("That target is not a container.");
  }

  return{ success, action_time };
}

Action::StateResult ActionPutInto::do_finish_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionPutInto::do_abort_work(AnyMap& params)
{
  return Action::StateResult::Success();
}