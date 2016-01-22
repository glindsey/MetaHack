#include "ActionClose.h"
#include "Thing.h"
#include "ThingRef.h"

ActionClose::ActionClose(ThingRef subject)
  :
  Action(subject)
{}

ActionClose::ActionClose(ThingRef subject, ThingRef object)
  :
  Action(subject, object)
{}

ActionClose::~ActionClose()
{}

Action::StateResult ActionClose::do_prebegin_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionClose::do_begin_work(AnyMap& params)
{
  bool success = false;
  unsigned int action_time = 0;

  the_message_log.add("We're sorry, but that action has not yet been implemented.");

#if 0
  if (thing != ThingManager::get_mu())
  {
    success = actor->do_close(thing, action_time);
  }
#endif

  return{ success, action_time };
}

Action::StateResult ActionClose::do_finish_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionClose::do_abort_work(AnyMap& params)
{
  return Action::StateResult::Success();
}