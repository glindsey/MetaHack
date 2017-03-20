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

  StateResult ActionUse::do_prebegin_work_(AnyMap& params)
  {
    // All checks done in Action class via traits.
    return StateResult::Success();
  }

  StateResult ActionUse::do_begin_work_(AnyMap& params)
  {
    StateResult result = StateResult::Failure();
    std::string message;
    auto subject = get_subject();
    auto object = get_object();

    /// @todo Figure out use time.
    print_message_begin_();
    result = StateResult::Success(1);

    return result;
  }

  StateResult ActionUse::do_finish_work_(AnyMap& params)
  {
    StateResult result = StateResult::Failure();
    std::string message;
    auto subject = get_subject();
    auto object = get_object();

    print_message_finish_();

    /// @todo Split read time into start/finish actions.
    switch (object->be_object_of(*this, subject))
    {
      case ActionResult::SuccessDestroyed:
        message = THE_FOO + OBJCV(" disintegrate", " disintegrates") + " after " + YOU + CV(" use ", " uses ") + OBJ_PRO_FOO + "!";
        Service<IMessageLog>::get().add(message);

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

  StateResult ActionUse::do_abort_work_(AnyMap& params)
  {
    auto subject = get_subject();
    auto object = get_object();

    print_message_stop_();

    return StateResult::Success();
  }
} // end namespace
