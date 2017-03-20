#include "stdafx.h"

#include "ActionWield.h"

#include "ActionUnwield.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

namespace Actions
{
  ActionWield ActionWield::prototype;
  ActionWield::ActionWield() : Action("wield", "WIELD", ActionWield::create_) {}
  ActionWield::ActionWield(EntityId subject) : Action(subject, "wield", "WIELD") {}
  ActionWield::~ActionWield() {}

  std::unordered_set<Trait> const & ActionWield::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObject,
      Trait::CanBeSubjectVerbObjectPrepositionBodypart,
      Trait::ObjectCanBeSelf,
      Trait::ObjectMustBeInInventory
    };

    return traits;
  }

  StateResult ActionWield::do_prebegin_work_(AnyMap& params)
  {
    std::string message;
    auto subject = get_subject();
    auto object = get_object();
    /// @todo Support wielding in other hand(s).
    unsigned int hand = 0;
    std::string bodypart_desc =
      subject->get_bodypart_description(BodyPart::Hand, hand);
    EntityId currently_wielded = subject->get_wielding_in(hand);

    std::string thing_name = (object != EntityId::Mu()) ? get_object_string_() : tr("NOUN_NOTHING");

    // If it is us, or it is what is already being wielded, it means to unwield whatever is wielded.
    if ((object == subject) || (object == currently_wielded) || (object == EntityId::Mu()))
    {
      std::unique_ptr<Action> unwieldAction(NEW ActionUnwield(subject));
      subject->queue_action(std::move(unwieldAction));

      return StateResult::Failure();
    }
    else if (currently_wielded != EntityId::Mu())
    {
      std::string message;
      message = maketr("YOU_MUST_UNWIELD_FIRST", { bodypart_desc });
      Service<IMessageLog>::get().add(message);

      return StateResult::Failure();
    }

    // Check that we have hands capable of wielding anything.
    if (subject->get_bodypart_number(BodyPart::Hand).as<uint32_t>() == 0)
    {
      print_message_try_();

      message = maketr("YOU_HAVE_NO_GRASPING_LIMBS");
      Service<IMessageLog>::get().add(message);

      return StateResult::Failure();
    }

    return StateResult::Success();
  }

  StateResult ActionWield::do_begin_work_(AnyMap& params)
  {
    StateResult result = StateResult::Failure();
    std::string message;
    auto subject = get_subject();
    auto object = get_object();

    /// @todo Support wielding in other hand(s). This will also include
    ///       shifting an already-wielded weapon to another hand.
    unsigned int hand = 0;
    std::string bodypart_desc =
      subject->get_bodypart_description(BodyPart::Hand, hand);
    EntityId currently_wielded = subject->get_wielding_in(hand);

    // If we HAVE a new item, try to wield it.
    if (object->be_object_of(*this, subject) == ActionResult::Success)
    {
      subject->set_wielded(object, hand);
      message = maketr("YOU_ARE_NOW_WIELDING", { get_object_string_(), subject->get_possessive_of(bodypart_desc) });
      Service<IMessageLog>::get().add(message);

      result = StateResult::Success();
    }

    return result;
  }

  StateResult ActionWield::do_finish_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionWield::do_abort_work_(AnyMap& params)
  {
    return StateResult::Success();
  }
} // end namespace
