#include "stdafx.h"

#include "ActionUnwield.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

namespace Actions
{
  ActionUnwield ActionUnwield::prototype;
  ActionUnwield::ActionUnwield() : Action("unwield", "UNWIELD", ActionUnwield::create_) {}
  ActionUnwield::ActionUnwield(EntityId subject) : Action(subject, "unwield", "UNWIELD") {}
  ActionUnwield::~ActionUnwield() {}

  std::unordered_set<Trait> const & ActionUnwield::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectOnly,
      Trait::CanBeSubjectVerbBodypart,
      Trait::ObjectMustBeWielded
    };

    return traits;
  }

  StateResult ActionUnwield::doPreBeginWorkNVI(AnyMap& params)
  {
    // All checks done by Action class via traits.
    return StateResult::Success();
  }

  StateResult ActionUnwield::doBeginWorkNVI(AnyMap& params)
  {
    StateResult result = StateResult::Failure();
    std::string message;

    auto subject = getSubject();
    auto object = getObject();

    BodyLocation wield_location;
    subject->is_wielding(object, wield_location);

    std::string bodypart_desc = subject->get_bodypart_description(wield_location);

    // Check if the wielded item is bound.
    if (object->get_modified_property("bound").as<bool>())
    {
      std::string message;
      message = makeString("$you cannot unwield $foo; it is magically bound to $0!", { subject->get_possessive_of(bodypart_desc) });
      Service<IMessageLog>::get().add(message);

      // Premature exit.
      return result;
    }

    // Try to unwield the item.
    /// @todo Unwielding shouldn't be instantaneous...?
    auto lua_result = object->be_object_of(*this, subject);
    if (object->be_object_of(*this, subject) == ActionResult::Success)
    {
      std::string message;
      message = makeString("$you unwield $foo. $you are now wielding nothing in $0.", { subject->get_possessive_of(bodypart_desc) });
      subject->set_wielded(EntityId::Mu(), wield_location);
    }

    return result;
  }

  StateResult ActionUnwield::doFinishWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionUnwield::doAbortWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }
} // end namespace

