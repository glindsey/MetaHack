#include "ActionMix.h"
#include "Thing.h"
#include "ThingRef.h"

ActionMix::ActionMix(ThingRef subject, std::vector<ThingRef> objects)
  :
  Action(subject, objects)
{}

ActionMix::~ActionMix()
{}

Action::StateResult ActionMix::do_prebegin_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionMix::do_begin_work(AnyMap& params)
{
  the_message_log.add("We're sorry, but that action has not yet been implemented.");

  return Action::StateResult::Failure();
}

Action::StateResult ActionMix::do_finish_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionMix::do_abort_work(AnyMap& params)
{
  return Action::StateResult::Success();
}