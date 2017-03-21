#include "stdafx.h"

#include "ActionWear.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

namespace Actions
{
  ActionWear ActionWear::prototype;
  ActionWear::ActionWear() : Action("wear", "WEAR", ActionWear::create_) {}
  ActionWear::ActionWear(EntityId subject) : Action(subject, "wear", "WEAR") {}
  ActionWear::~ActionWear() {}

  std::unordered_set<Trait> const & ActionWear::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObjectPrepositionBodypart,
      Trait::ObjectMustBeInInventory
    };

    return traits;
  }

  StateResult ActionWear::do_prebegin_work_(AnyMap& params)
  {
    auto subject = get_subject();
    auto object = get_object();

    auto bodypart = object->is_equippable_on();

    if (bodypart == BodyPart::Count)
    {
      put_msg(maketr("THE_FOO_IS_NOT_VERBABLE"));
      return StateResult::Failure();
    }
    else
    {
      /// @todo Check that entity has free body part(s) to equip item on.
      m_body_location.part = bodypart;
      m_body_location.number = 0;
    }

    return StateResult::Success();
  }

  StateResult ActionWear::do_begin_work_(AnyMap& params)
  {
    auto subject = get_subject();
    auto object = get_object();

    // Do the action here.
    if (object->be_object_of(*this, subject) == ActionResult::Success)
    {
      /// @todo Figure out action time.
      return StateResult::Success();
    }

    return StateResult::Failure();
  }

  StateResult ActionWear::do_finish_work_(AnyMap& params)
  {
    auto subject = get_subject();
    auto object = get_object();

    std::string bodypart_desc =
      subject->get_bodypart_description(m_body_location);

    subject->set_worn(object, m_body_location);
    put_msg(maketr("YOU_ARE_NOW_WEARING_THE_FOO", { subject->get_possessive_of(bodypart_desc) }));

    return StateResult::Success();
  }

  StateResult ActionWear::do_abort_work_(AnyMap& params)
  {
    return StateResult::Success();
  }
}
