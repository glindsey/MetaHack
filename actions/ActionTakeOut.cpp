#include "stdafx.h"

#include "ActionTakeOut.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

namespace Actions
{
  ActionTakeOut ActionTakeOut::prototype;
  ActionTakeOut::ActionTakeOut() : Action("remove", "REMOVE", ActionTakeOut::create_) {}
  ActionTakeOut::ActionTakeOut(EntityId subject) : Action(subject, "remove", "REMOVE") {}
  ActionTakeOut::~ActionTakeOut() {}

  std::unordered_set<Trait> const & ActionTakeOut::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObject,
      Trait::CanBeSubjectVerbObjects,
      Trait::ObjectCanBeOutOfReach
    };

    return traits;
  }

  StateResult ActionTakeOut::doPreBeginWorkNVI(AnyMap& params)
  {
    std::string message;
    auto subject = getSubject();
    auto object = getObject();
    auto container = object->getLocation();

    // Check that the container is not a MapTile or DynamicEntity.
    if (!object->isInsideAnotherEntity())
    {
      printMessageTry();

      //message = YOU_TRY + " to remove " + THE_FOO +
      //  " from its container.";
      //Service<IMessageLog>::get().add(message);

      if (IS_PLAYER)
      {
        message = maketr("CONJUNCTION_BUT") + " ";
      }

      message += maketr("THE_FOO_IS_NOT_PREPOSITION_NOUN", 
      { 
        maketr("PREPOSITION_INSIDE"), 
        getIndefArt(maketr("NOUN_CONTAINER"))
      });

      Service<IMessageLog>::get().add(message);

      return StateResult::Failure();
    }

    // Check that the container is within reach.
    if (!subject->canReach(container))
    {
      printMessageTry();

      put_msg(maketr("THE_FOO_IS_OUT_OF_REACH"));
      return StateResult::Failure();
    }

    return StateResult::Success();
  }

  StateResult ActionTakeOut::doBeginWorkNVI(AnyMap& params)
  {
    /// @todo Handle taking out a certain quantity of an item.
    StateResult result = StateResult::Failure();
    std::string message;
    auto subject = getSubject();
    auto object = getObject();
    auto container = object->getLocation();
    auto new_location = container->getLocation();

    // Set the target to be the container as a kludge for message printing.
    setTarget(container);

    if (object->be_object_of(*this, subject) == ActionResult::Success)
    {
      if (object->move_into(new_location))
      {
        printMessageDo();

        /// @todo Figure out action time.
        result = StateResult::Success();
      }
      else
      {
        message = YOU + " could not take " + getObjectString() + " out of " + getTargetString() + " for some inexplicable reason.";
        Service<IMessageLog>::get().add(message);

        CLOG(ERROR, "Action") << "Could not move Entity out of Container even though be_object_of returned Success";
      }
    }

    return result;
  }

  StateResult ActionTakeOut::doFinishWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionTakeOut::doAbortWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }

  void ActionTakeOut::printMessageTry() const
  {
    put_msg(maketr("YOU_TRY_TO_VERB_THE_FOO_PREPOSITION_TARGET", { "from" }));
  }

  void ActionTakeOut::printMessageDo() const
  {
    put_msg(maketr("YOU_VERB_THE_FOO_PREPOSITION_TARGET", { "from" }));
  }
}
