#include "stdafx.h"

#include "ActionRead.h"
#include "IMessageLog.h"
#include "IStringDictionary.h"
#include "Service.h"
#include "Entity.h"
#include "EntityId.h"

namespace Actions
{
  ActionRead ActionRead::prototype;
  ActionRead::ActionRead() : Action("read", "READ", ActionRead::create_) {}
  ActionRead::ActionRead(EntityId subject) : Action(subject, "read", "READ") {}
  ActionRead::~ActionRead() {}

  std::unordered_set<Trait> const & ActionRead::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObject,
      Trait::CanBeSubjectVerbDirection
    };

    return traits;
  }

  StateResult ActionRead::do_prebegin_work_(AnyMap& params)
  {
    std::string message;
    auto subject = get_subject();
    auto object = get_object();

    if (false) ///< @todo Intelligence tests for reading.
    {
      print_message_try_();

      message = maketr("READ_TOO_STUPID");
      Service<IMessageLog>::get().add(message);

      return StateResult::Failure();
    }

    return StateResult::Success();
  }

  StateResult ActionRead::do_begin_work_(AnyMap& params)
  {
    StateResult result = StateResult::Failure();
    std::string message;
    auto subject = get_subject();
    auto object = get_object();

    /// @todo Figure out read time.
    print_message_begin_();
    result = StateResult::Success(1);

    return result;
  }

  StateResult ActionRead::do_finish_work_(AnyMap& params)
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
        message = make_string("$the_foo $(objcv?disintegrate:disintegrates) after $you $cverb $foo_pro_obj!");
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
  }

  StateResult ActionRead::do_abort_work_(AnyMap& params)
  {
    auto subject = get_subject();
    auto object = get_object();

    print_message_stop_();
    return StateResult::Success();
  }

  void ActionRead::print_message_cant_() const
  {
    std::string message = make_string("$the_foo $foo_has no writing to read.");
    Service<IMessageLog>::get().add(message);
  }
} // end namespace
