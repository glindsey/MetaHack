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

  StateResult ActionHurl::do_prebegin_work_(AnyMap& params)
  {
    std::string message;
    auto subject = get_subject();
    auto object = get_object();
    auto new_direction = get_target_direction();

    return StateResult::Success();
  }

  StateResult ActionHurl::do_begin_work_(AnyMap& params)
  {
    auto result = StateResult::Failure();
    std::string message;
    auto subject = get_subject();
    auto object = get_object();
    auto direction = get_target_direction();
    EntityId new_location = subject->getLocation();

    if (object->be_object_of(*this, subject, direction) == ActionResult::Success)
    {
      if (object->move_into(new_location))
      {
        print_message_do_();

        /// @todo When throwing, set Entity's direction and velocity
        /// @todo Figure out action time.
        result = StateResult::Success();
      }
      else
      {
        message = maketr("YOU_CANT_VERB_FOO_UNKNOWN");
        Service<IMessageLog>::get().add(message);

        CLOG(WARNING, "Action") << "Could not throw Entity " << object <<
          " even though be_object_of returned Success";
      }
    }

    return result;
  }

  StateResult ActionHurl::do_finish_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionHurl::do_abort_work_(AnyMap& params)
  {
    return StateResult::Success();
  }
} // end namespace
