#include "stdafx.h"

#include "ActionQuaff.h"
#include "IMessageLog.h"
#include "IStringDictionary.h"
#include "Service.h"
#include "Entity.h"
#include "EntityId.h"

namespace Actions
{
  ActionQuaff ActionQuaff::prototype;
  ActionQuaff::ActionQuaff() : Action("drink", "DRINK", ActionQuaff::create_) {}
  ActionQuaff::ActionQuaff(EntityId subject) : Action(subject, "drink", "DRINK") {}
  ActionQuaff::~ActionQuaff() {}

  std::unordered_set<Trait> const & ActionQuaff::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObject,
      Trait::CanBeSubjectVerbDirection,
      Trait::ObjectCanBeSelf,
      Trait::ObjectMustNotBeEmpty,
      Trait::ObjectMustBeLiquidCarrier
    };

    return traits;
  }

  StateResult ActionQuaff::do_prebegin_work_(AnyMap& params)
  {
    // All checks handled by Action class via traits.
    return StateResult::Success();
  }

  StateResult ActionQuaff::do_begin_work_(AnyMap& params)
  {
    auto subject = get_subject();
    auto object = get_object();
    auto contents = object->get_inventory()[INVSLOT_ZERO];

    print_message_begin_();

    // Do the drinking action here.
    /// @todo We drink from the object, but it's what is inside that is
    ///       actually being consumed. Do we call be_object_of() on the
    ///       object, or on the object's contents?
    ///       For now I am assuming we do it on the contents, since they
    ///       are what will affect the drinker.
    /// @todo Figure out drinking time. This will vary based on the contents
    ///       being consumed.
    ActionResult result = contents->be_object_of(*this, subject);

    switch (result)
    {
      case ActionResult::Success:
        return StateResult::Success();

      case ActionResult::SuccessDestroyed:
        contents->destroy();
        return StateResult::Success();

      case ActionResult::Failure:
        print_message_stop_();
        return StateResult::Failure();

      default:
        CLOG(WARNING, "Action") << "Unknown ActionResult " << result;
        return StateResult::Failure();
    }
  }

  StateResult ActionQuaff::do_finish_work_(AnyMap& params)
  {
    auto object = get_object();

    print_message_finish_();
    return StateResult::Success();
  }

  StateResult ActionQuaff::do_abort_work_(AnyMap& params)
  {
    print_message_stop_();
    return StateResult::Success();
  }
} // end namespace

