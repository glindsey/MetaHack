#include "stdafx.h"

#include "ActionPutInto.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

namespace Actions
{
  ActionPutInto ActionPutInto::prototype;
  ActionPutInto::ActionPutInto() : Action("putinto", "STORE", ActionPutInto::create_) {}
  ActionPutInto::ActionPutInto(EntityId subject) : Action(subject, "putinto", "STORE") {}
  ActionPutInto::~ActionPutInto() {}

  std::unordered_set<Trait> const & ActionPutInto::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObjectPrepositionTarget,
      Trait::CanBeSubjectVerbObjectsPrepositionTarget,
      Trait::ObjectMustNotBeWielded,
      Trait::ObjectMustNotBeWorn
    };

    return traits;
  }

  StateResult ActionPutInto::doPreBeginWorkNVI(AnyMap& params)
  {
    std::string message;
    auto subject = getSubject();
    auto object = getObject();
    auto container = getTargetThing();

    // Verify that the Action has an object.
    if (object == EntityId::Mu())
    {
      return StateResult::Failure();
    }

    // Check that the entity and container aren't the same entity.
    if (object == container)
    {
      if (subject->isPlayer())
      {
        message = makeTr("YOU_TRY_TO_STORE_THE_FOO_INSIDE_ITSELF_HUMOROUS");
      }
      else
      {
        message = makeTr("YOU_TRY_TO_STORE_THE_FOO_INSIDE_ITSELF_INVALID");
        CLOG(WARNING, "Action") << "NPC tried to store a container in itself!?";
      }

      return StateResult::Failure();
    }

    // Check that the container actually IS a container.
    if (container->getIntrinsic("inventory_size").as<int32_t>() == 0)
    {
      printMessageTry();
      putTr("THE_TARGET_IS_NOT_A_CONTAINER");
      return StateResult::Failure();
    }

    // Check that the entity's location isn't already the container.
    if (object->getLocation() == container)
    {
      printMessageTry();
      putTr("THE_FOO_IS_ALREADY_IN_THE_TARGET");
      return StateResult::Failure();
    }

    // Check that the container is within reach.
    if (!subject->canReach(container))
    {
      printMessageTry();
      putTr("THE_TARGET_IS_OUT_OF_REACH");
      return StateResult::Failure();
    }

    return StateResult::Success();
  }

  StateResult ActionPutInto::doBeginWorkNVI(AnyMap& params)
  {
    /// @todo Handle putting a certain quantity of an item.
    StateResult result = StateResult::Failure();
    std::string message;
    auto subject = getSubject();
    auto object = getObject();
    auto container = getTargetThing();

    if (object->be_object_of(*this, subject, container) == ActionResult::Success)
    {
      printMessageDo();

      if (object->move_into(container))
      {
        /// @todo Figure out action time.
        result = StateResult::Success();
      }
      else
      {
        putMsg(makeTr("YOU_CANT_VERB_FOO_PREPOSITION_TARGET_UNKNOWN", { "into" }));

        CLOG(ERROR, "Action") << "Could not move Entity into Container even though be_object_of returned Success";
      }
    }

    return result;
  }

  StateResult ActionPutInto::doFinishWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionPutInto::doAbortWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }

  void ActionPutInto::printMessageTry() const
  {
    putMsg(makeTr("YOU_TRY_TO_VERB_THE_FOO_PREPOSITION_TARGET", { "into" }));
  }

  void ActionPutInto::printMessageDo() const
  {
    putMsg(makeTr("YOU_VERB_THE_FOO_PREPOSITION_TARGET", { "into" }));
  }
} // end namespace
