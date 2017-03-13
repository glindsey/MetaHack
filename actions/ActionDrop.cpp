#include "stdafx.h"

#include "ActionDrop.h"
#include "ActionMove.h"
#include "IMessageLog.h"
#include "IStringDictionary.h"
#include "Service.h"
#include "Entity.h"
#include "EntityId.h"

namespace Actions
{
  ActionDrop ActionDrop::prototype;
  ActionDrop::ActionDrop() : Action("drop", "DROP", ActionDrop::create_) {}
  ActionDrop::ActionDrop(EntityId subject) : Action(subject, "drop", "DROP") {}
  ActionDrop::~ActionDrop() {}

  std::unordered_set<Trait> const & ActionDrop::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObject,
      Trait::CanTakeAQuantity
    };

    return traits;
  }
  StateResult ActionDrop::do_prebegin_work_(AnyMap& params)
  {
    std::string message;
    auto subject = get_subject();
    auto object = get_object();
    EntityId location = subject->getLocation();

    // If it's us, this is a special case. Return success.
    if (subject == object)
    {
      return StateResult::Success();
    }

    // Check that it's in our inventory.
    if (!subject->get_inventory().contains(object))
    {
      print_message_try_();

      message = maketr("ACTION_FOO_NOT_IN_INVENTORY");
      Service<IMessageLog>::get().add(message);

      return StateResult::Failure();
    }

    // Check that we're not wielding the item.
    if (subject->is_wielding(object))
    {
      print_message_try_();

      /// @todo Perhaps automatically try to unwield the item before dropping?
      message = maketr("ACTION_YOU_CANT_VERB_WIELDED");
      Service<IMessageLog>::get().add(message);

      return StateResult::Failure();
    }

    // Check that we're not wearing the item.
    if (subject->has_equipped(object))
    {
      print_message_try_();

      message = maketr("ACTION_YOU_CANT_VERB_WORN"); 
      Service<IMessageLog>::get().add(message);

      return StateResult::Failure();
    }

    // Check that we can move the item.
    if (!object->can_have_action_done_by(subject, ActionMove::prototype))
    {
      print_message_try_();

      message = maketr("ACTION_FOO_CANT_BE_VERBED");
      Service<IMessageLog>::get().add(message);

      return StateResult::Failure();
    }

    return StateResult::Success();
  }

  StateResult ActionDrop::do_begin_work_(AnyMap& params)
  {
    StateResult result = StateResult::Failure();
    std::string message;
    auto subject = get_subject();
    auto object = get_objects().front();
    EntityId location = subject->getLocation();

    /// @todo Handle dropping a certain quantity of an item.

    if (object == subject)
    {
      message = maketr("ACTION_YOU_THROW_SELF_TO_GROUND", { location->get_display_name() });
      Service<IMessageLog>::get().add(message);
      /// @todo Possible damage from hurling yourself to the ground!
      message = maketr("YOU_SEEM_UNHARMED", { IS_PLAYER ? tr("PREFIX_FORTUNATELY") : "" });
      Service<IMessageLog>::get().add(message);
      message = maketr("YOU_GET_UP");
      Service<IMessageLog>::get().add(message);
    }
    else if (object != EntityId::Mu())
    {
      if (location->can_contain(object) == ActionResult::Success)
      {
        if (object->be_object_of(*this, subject) == ActionResult::Success)
        {
          print_message_do_();

          if (object->move_into(location))
          {
            /// @todo Figure out action time.
            result = StateResult::Success();
          }
          else
          {
            message = maketr("ACTION_YOU_CANT_VERB_FOO_UNKNOWN");
            Service<IMessageLog>::get().add(message);

            CLOG(WARNING, "Action") << "Could not drop Entity " << object <<
              " even though be_object_of returned Success";
          }
        }
        else // Drop failed
        {
          // be_object_of() will print any relevant messages
        }
      }
      else // can't contain the entity
      {
        // This is mighty strange, but I suppose there might be MapTiles in
        // the future that can't contain certain Entities.
        print_message_try_();

        message = maketr("LOCATION_CANT_HOLD_FOO", { location->get_identifying_string() });
        Service<IMessageLog>::get().add(message);
      }
    }

    return result;
  }

  StateResult ActionDrop::do_finish_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionDrop::do_abort_work_(AnyMap& params)
  {
    return StateResult::Success();
  }
} // end namespace
