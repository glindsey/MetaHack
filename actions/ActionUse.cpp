#include "stdafx.h"

#include "ActionUse.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

namespace Actions
{
  ActionUse ActionUse::prototype;
  ActionUse::ActionUse() : Action("use", "USE", ActionUse::create_) {}
  ActionUse::ActionUse(EntityId subject) : Action(subject, "use", "USE") {}
  ActionUse::~ActionUse() {}

  std::unordered_set<Trait> const & ActionUse::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObject
    };

    return traits;
  }

  StateResult ActionUse::doPreBeginWorkNVI(AnyMap& params)
  {
    // All checks done in Action class via traits.
    return StateResult::Success();
  }

  StateResult ActionUse::doBeginWorkNVI(AnyMap& params)
  {
    StateResult result = StateResult::Failure();
    std::string message;
    auto subject = getSubject();
    auto object = getObject();

    /// @todo Figure out use time.
    printMessageBegin();
    result = StateResult::Success(1);

    return result;
  }

  StateResult ActionUse::doFinishWorkNVI(AnyMap& params)
  {
    StateResult result = StateResult::Failure();
    std::string message;
    auto subject = getSubject();
    auto object = getObject();

    printMessageFinish();

    /// @todo Split read time into start/finish actions.
    switch (object->be_object_of(*this, subject))
    {
      case ActionResult::SuccessDestroyed:
        putTr("THE_FOO_DISINTEGRATES_AFTER_YOU_VERB");

        object->destroy();
        result = StateResult::Success();
        break;

      case ActionResult::Success:
        result = StateResult::Success();
        break;

      case ActionResult::Failure:
      default:
        result = StateResult::Failure();
        break;
    }

    return result;

    return StateResult::Success();
  }

  StateResult ActionUse::doAbortWorkNVI(AnyMap& params)
  {
    auto subject = getSubject();
    auto object = getObject();

    printMessageStop();

    return StateResult::Success();
  }
} // end namespace
