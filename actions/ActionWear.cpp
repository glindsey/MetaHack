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

  StateResult ActionWear::doPreBeginWorkNVI(AnyMap& params)
  {
    auto subject = getSubject();
    auto object = getObject();

    auto bodypart = object->is_equippable_on();

    if (bodypart == BodyPart::Count)
    {
      putTr("THE_FOO_IS_NOT_VERBABLE");
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

  StateResult ActionWear::doBeginWorkNVI(AnyMap& params)
  {
    auto subject = getSubject();
    auto object = getObject();

    // Do the action here.
    if (object->beObjectOf(*this, subject))
    {
      /// @todo Figure out action time.
      return StateResult::Success();
    }

    return StateResult::Failure();
  }

  StateResult ActionWear::doFinishWorkNVI(AnyMap& params)
  {
    auto subject = getSubject();
    auto object = getObject();

    std::string bodypart_desc =
      subject->getBodypartDescription(m_body_location);

    subject->setWorn(object, m_body_location);
    putMsg(makeTr("YOU_ARE_NOW_WEARING_THE_FOO", { subject->getPossessiveString(bodypart_desc) }));

    return StateResult::Success();
  }

  StateResult ActionWear::doAbortWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }
}
