#include "stdafx.h"

#include "ActionTakeOff.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

namespace Actions
{
  ActionTakeOff ActionTakeOff::prototype;
  ActionTakeOff::ActionTakeOff() : Action("disrobe", "DISROBE", ActionTakeOff::create_) {}
  ActionTakeOff::ActionTakeOff(EntityId subject) : Action(subject, "disrobe", "DISROBE") {}
  ActionTakeOff::~ActionTakeOff() {}

  std::unordered_set<Trait> const & ActionTakeOff::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbBodypart,
      Trait::ObjectMustBeWorn
    };

    return traits;
  }

  StateResult ActionTakeOff::do_prebegin_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionTakeOff::do_begin_work_(AnyMap& params)
  {
    StateResult result = StateResult::Failure();
    std::string message;

    auto subject = get_subject();
    auto object = get_object();

    BodyLocation wear_location;
    subject->is_wearing(object, wear_location);

    std::string bodypart_desc = subject->get_bodypart_description(wear_location);

    // Check if the worn item is bound.
    if (object->get_modified_property("bound").as<bool>())
    {
      std::string message;
      message = make_string("$you cannot take off $foo; it is magically bound to $0!", { subject->get_possessive_of(bodypart_desc) });
      Service<IMessageLog>::get().add(message);

      // Premature exit.
      return result;
    }

    // Try to take off the item.
    /// @todo Disrobing shouldn't be instantaneous.
    auto lua_result = object->be_object_of(*this, subject);
    if (object->be_object_of(*this, subject) == ActionResult::Success)
    {
      std::string message;
      message = make_string("$you take off $foo. $you are now wearing nothing on $0.", { subject->get_possessive_of(bodypart_desc) });
      subject->set_wielded(EntityId::Mu(), wear_location);
    }

    return result;
  }

  StateResult ActionTakeOff::do_finish_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionTakeOff::do_abort_work_(AnyMap& params)
  {
    return StateResult::Success();
  }
}
