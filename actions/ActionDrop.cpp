#include "stdafx.h"

#include "ActionDrop.h"
#include "ActionMove.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

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
      Trait::CanTakeAQuantity,
      Trait::ObjectMustNotBeWielded,
      Trait::ObjectMustNotBeWorn,
      Trait::ObjectMustBeInInventory,
      Trait::ObjectMustBeMovableBySubject
    };

    return traits;
  }

  StateResult ActionDrop::doPreBeginWorkNVI(AnyMap& params)
  {
    // All checks handled in Action class via traits.
    return StateResult::Success();
  }

  StateResult ActionDrop::doBeginWorkNVI(AnyMap& params)
  {
    StateResult result = StateResult::Failure();
    std::string message;
    auto subject = getSubject();
    auto object = getObjects().front();
    EntityId location = subject->getLocation();

    /// @todo Handle dropping a certain quantity of an item.

    if (object == subject)
    {
      putMsg(makeTr("YOU_THROW_SELF_TO_GROUND", { location->getDisplayName() }));
      /// @todo Possible damage from hurling yourself to the ground!
      putMsg(makeTr("YOU_SEEM_UNHARMED", { subject->isPlayer() ? tr("PREFIX_FORTUNATELY") : "" }));
      putTr("YOU_GET_UP");
    }
    else if (object != EntityId::Mu())
    {
      if (location->can_contain(object) == ActionResult::Success)
      {
        if (object->be_object_of(*this, subject) == ActionResult::Success)
        {
          printMessageDo();

          if (object->move_into(location))
          {
            /// @todo Figure out action time.
            result = StateResult::Success();
          }
          else
          {
            putTr("YOU_CANT_VERB_FOO_UNKNOWN");

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
        printMessageTry();

        putMsg(makeTr("LOCATION_CANT_HOLD_FOO", { location->getDescriptiveString() }));
      }
    }

    return result;
  }

  StateResult ActionDrop::doFinishWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionDrop::doAbortWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }
} // end namespace
