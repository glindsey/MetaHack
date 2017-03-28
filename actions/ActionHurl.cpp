#include "stdafx.h"

#include "ActionHurl.h"
#include "services/IMessageLog.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"


namespace Actions
{
  ActionHurl ActionHurl::prototype;
  ActionHurl::ActionHurl() : Action("hurl", "THROW", ActionHurl::create_) {}
  ActionHurl::ActionHurl(EntityId subject) : Action(subject, "hurl", "THROW") {}
  ActionHurl::~ActionHurl() {}

  std::unordered_set<Trait> const & ActionHurl::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObjectPrepositionDirection,
      Trait::ObjectMustNotBeWorn,
      Trait::ObjectMustBeInInventory
    };

    return traits;
  }

  StateResult ActionHurl::doPreBeginWorkNVI(AnyMap& params)
  {
    std::string message;
    auto subject = getSubject();
    auto object = getObject();
    auto new_direction = getTargetDirection();

    return StateResult::Success();
  }

  StateResult ActionHurl::doBeginWorkNVI(AnyMap& params)
  {
    auto result = StateResult::Failure();
    std::string message;
    auto subject = getSubject();
    auto object = getObject();
    auto direction = getTargetDirection();
    EntityId new_location = subject->getLocation();

    if (object->be_object_of(*this, subject, direction) == ActionResult::Success)
    {
      if (object->move_into(new_location))
      {
        printMessageDo();

        /// @todo When throwing, set Entity's direction and velocity
        /// @todo Figure out action time.
        result = StateResult::Success();
      }
      else
      {
        putTr("YOU_CANT_VERB_FOO_UNKNOWN");

        CLOG(WARNING, "Action") << "Could not throw Entity " << object <<
          " even though be_object_of returned Success";
      }
    }

    return result;
  }

  StateResult ActionHurl::doFinishWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionHurl::doAbortWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }
} // end namespace
