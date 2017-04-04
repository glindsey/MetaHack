#include "stdafx.h"

#include "ActionQuaff.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

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

  StateResult ActionQuaff::doPreBeginWorkNVI(AnyMap& params)
  {
    // All checks handled by Action class via traits.
    return StateResult::Success();
  }

  StateResult ActionQuaff::doBeginWorkNVI(AnyMap& params)
  {
    auto subject = getSubject();
    auto object = getObject();
    auto contents = object->getInventory()[INVSLOT_ZERO];

    printMessageBegin();

    // Do the drinking action here.
    /// @todo We drink from the object, but it's what is inside that is
    ///       actually being consumed. Do we call be_object_of() on the
    ///       object, or on the object's contents?
    ///       For now I am assuming we do it on the contents, since they
    ///       are what will affect the drinker.
    /// @todo Figure out drinking time. This will vary based on the contents
    ///       being consumed.
    if (contents->be_object_of(*this, subject))
    {
      contents->destroy();
      return StateResult::Success();
    }
    else
    {
      printMessageStop();
      return StateResult::Failure();
    }
  }

  StateResult ActionQuaff::doFinishWorkNVI(AnyMap& params)
  {
    auto object = getObject();

    printMessageFinish();
    return StateResult::Success();
  }

  StateResult ActionQuaff::doAbortWorkNVI(AnyMap& params)
  {
    printMessageStop();
    return StateResult::Success();
  }
} // end namespace

