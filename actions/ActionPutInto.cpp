#include "stdafx.h"

#include "ActionPutInto.h"
#include "IMessageLog.h"
#include "IStringDictionary.h"
#include "Service.h"
#include "Entity.h"
#include "EntityId.h"

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
        message = "That would be an interesting topological exercise.";
      }
      else
      {
        message = make_string("$you $try to store $the_foo inside itself, which seriously shouldn't happen.");
        CLOG(WARNING, "Action") << "NPC tried to store a container in itself!?";
      }

      return StateResult::Failure();
    }

    // Check that the container actually IS a container.
    if (container->get_intrinsic<int>("inventory_size") == 0)
    {
      print_message_try_();

      message = make_string("$the_target_thing is not a container!");
      Service<IMessageLog>::get().add(message);

      return StateResult::Failure();
    }

    // Check that the entity's location isn't already the container.
    if (object->getLocation() == container)
    {
      print_message_try_();

      message = make_string("$the_foo is already in $the_target_thing!");
      Service<IMessageLog>::get().add(message);

      return StateResult::Failure();
    }

    // Check that the container is within reach.
    if (!subject->can_reach(container))
    {
      print_message_try_();

      message = make_string("$you cannot reach $the_target_thing.");
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
        message = make_string("$you could not move $the_foo into $the_target_thing for some inexplicable reason.");
        Service<IMessageLog>::get().add(message);

        MAJOR_ERROR("Could not move Entity into Container even though be_object_of returned Success");
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
    std::string message = make_string("$you $try to $verb $the_foo into $the_target_thing.");
    Service<IMessageLog>::get().add(message);
  }

  void ActionPutInto::print_message_do_() const
  {
    std::string message = make_string("$you $cverb $the_foo into $the_target_thing.");
    Service<IMessageLog>::get().add(message);
  }
} // end namespace
