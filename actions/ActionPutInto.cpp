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

  StateResult ActionPutInto::do_prebegin_work_(AnyMap& params)
  {
    std::string message;
    auto subject = get_subject();
    auto object = get_object();
    auto container = get_target_thing();

    // Verify that the Action has an object.
    if (object == EntityId::Mu())
    {
      return StateResult::Failure();
    }

    // Check that the entity and container aren't the same entity.
    if (object == container)
    {
      if (IS_PLAYER)
      {
        message = maketr("YOU_TRY_TO_STORE_THE_FOO_INSIDE_ITSELF_HUMOROUS");
      }
      else
      {
        message = maketr("YOU_TRY_TO_STORE_THE_FOO_INSIDE_ITSELF_INVALID");
        CLOG(WARNING, "Action") << "NPC tried to store a container in itself!?";
      }

      return StateResult::Failure();
    }

    // Check that the container actually IS a container.
    if (container->get_intrinsic("inventory_size").as<int32_t>() == 0)
    {
      print_message_try_();

      message = maketr("THE_TARGET_IS_NOT_A_CONTAINER");
      Service<IMessageLog>::get().add(message);

      return StateResult::Failure();
    }

    // Check that the entity's location isn't already the container.
    if (object->getLocation() == container)
    {
      print_message_try_();

      message = maketr("THE_FOO_IS_ALREADY_IN_THE_TARGET");
      Service<IMessageLog>::get().add(message);

      return StateResult::Failure();
    }

    // Check that the container is within reach.
    if (!subject->can_reach(container))
    {
      print_message_try_();

      message = maketr("THE_TARGET_IS_OUT_OF_REACH");
      Service<IMessageLog>::get().add(message);

      return StateResult::Failure();
    }

    return StateResult::Success();
  }

  StateResult ActionPutInto::do_begin_work_(AnyMap& params)
  {
    /// @todo Handle putting a certain quantity of an item.
    StateResult result = StateResult::Failure();
    std::string message;
    auto subject = get_subject();
    auto object = get_object();
    auto container = get_target_thing();

    if (object->be_object_of(*this, subject, container) == ActionResult::Success)
    {
      print_message_do_();

      if (object->move_into(container))
      {
        /// @todo Figure out action time.
        result = StateResult::Success();
      }
      else
      {
        message = maketr("YOU_CANT_VERB_FOO_PREPOSITION_TARGET_UNKNOWN", { "into" });
        Service<IMessageLog>::get().add(message);

        CLOG(ERROR, "Action") << "Could not move Entity into Container even though be_object_of returned Success";
      }
    }

    return result;
  }

  StateResult ActionPutInto::do_finish_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionPutInto::do_abort_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  void ActionPutInto::print_message_try_() const
  {
    std::string message = maketr("YOU_TRY_TO_VERB_THE_FOO_PREPOSITION_TARGET", { "into" });
    Service<IMessageLog>::get().add(message);
  }

  void ActionPutInto::print_message_do_() const
  {
    std::string message = maketr("YOU_VERB_THE_FOO_PREPOSITION_TARGET", { "into" });
    Service<IMessageLog>::get().add(message);
  }
} // end namespace
