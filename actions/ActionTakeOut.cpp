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

      if (subject->isPlayer())
      {
        message = makeTr("CONJUNCTION_BUT") + " ";
      }

      message += makeTr("THE_FOO_IS_NOT_PREPOSITION_NOUN", 
      { 
        makeTr("PREPOSITION_INSIDE"), 
        getIndefArt(makeTr("NOUN_CONTAINER"))
      });

      putMsg(message);

      return StateResult::Failure();
    }

    // Check that the container is within reach.
    if (!subject->canReach(container))
    {
      printMessageTry();

      putTr("THE_FOO_IS_OUT_OF_REACH");
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

    if (object->be_object_of(*this, subject))
    {
      if (object->move_into(new_location))
      {
        printMessageDo();

        /// @todo Figure out action time.
        result = StateResult::Success();
      }
      else
      {
        putMsg(makeTr("YOU_CANT_VERB_FOO_PREPOSITION_TARGET_UNKNOWN", { "from" }));
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
    putMsg(makeTr("YOU_TRY_TO_VERB_THE_FOO_PREPOSITION_TARGET", { "from" }));
  }

  void ActionTakeOut::printMessageDo() const
  {
    putMsg(makeTr("YOU_VERB_THE_FOO_PREPOSITION_TARGET", { "from" }));
  }
}
