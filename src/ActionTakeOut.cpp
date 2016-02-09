#include "ActionTakeOut.h"
#include "Thing.h"
#include "ThingRef.h"

ActionTakeOut::ActionTakeOut(ThingRef subject, ThingRef object)
  :
  Action(subject, object)
{}

ActionTakeOut::ActionTakeOut(ThingRef subject, std::vector<ThingRef> objects)
  :
  Action(subject, objects)
{}

ActionTakeOut::~ActionTakeOut()
{}

Action::StateResult ActionTakeOut::do_prebegin_work(AnyMap& params)
{
  std::string message;
  auto subject = get_subject();
  auto object = get_object();
  auto container = object->get_location();

  // Verify that the Action has an object.
  if (object == ThingManager::get_mu())
  {
    return StateResult::Failure();
  }

  // Check that the thing isn't US!
  if (object == subject)
  {
    return StateResult::Failure(); //ActionResult::FailureSelfReference;
  }

  // Check that the container is not a MapTile or Entity.
  if (!object->is_inside_another_thing())
  {
    return StateResult::Failure(); //ActionResult::FailureNotInsideContainer;
  }

  // Check that the container is within reach.
  if (!subject->can_reach(container))
  {
    return StateResult::Failure(); //ActionResult::FailureContainerOutOfReach;
  }

  return StateResult::Success();
}

Action::StateResult ActionTakeOut::do_begin_work(AnyMap& params)
{
  bool success = false;
  unsigned int action_time;

  ThingRef thing = get_objects().front();
  if (thing != ThingManager::get_mu())
  {
    success = get_subject()->do_take_out(thing, action_time);
  }

  return{ success, action_time };
}

Action::StateResult ActionTakeOut::do_finish_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionTakeOut::do_abort_work(AnyMap& params)
{
  return Action::StateResult::Success();
}