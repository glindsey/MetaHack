#include "stdafx.h"

#include "ActionRead.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

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

  StateResult ActionRead::doPreBeginWorkNVI(AnyMap& params)
  {
    std::string message;
    auto subject = getSubject();
    auto object = getObject();

    if (false) ///< @todo Intelligence tests for reading.
    {
      printMessageTry();

      message = maketr("READ_TOO_STUPID");
      Service<IMessageLog>::get().add(message);

      return StateResult::Failure();
    }

    return StateResult::Success();
  }

  StateResult ActionRead::doBeginWorkNVI(AnyMap& params)
  {
    StateResult result = StateResult::Failure();
    std::string message;
    auto subject = getSubject();
    auto object = getObject();

    /// @todo Figure out read time.
    printMessageBegin();
    result = StateResult::Success(1);

    return result;
  }

  StateResult ActionRead::doFinishWorkNVI(AnyMap& params)
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
        message = maketr("THE_FOO_DISINTEGRATES_AFTER_YOU_VERB");
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

  StateResult ActionRead::doAbortWorkNVI(AnyMap& params)
  {
    auto subject = getSubject();
    auto object = getObject();

    printMessageStop();
    return StateResult::Success();
  }

  void ActionRead::printMessageCant() const
  {
    std::string message = maketr("THE_FOO_HAS_NO_NOUN_TO_VERB", { "writing" });
    Service<IMessageLog>::get().add(message);
  }
} // end namespace
