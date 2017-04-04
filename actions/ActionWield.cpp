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

  StateResult ActionWield::doPreBeginWorkNVI(AnyMap& params)
  {
    std::string message;
    auto subject = getSubject();
    auto object = getObject();

    /// @todo Support wielding in other hand(s). This will also include
    ///       shifting an already-wielded weapon to another hand.
    /// @todo Support wielding in ANY prehensile limb (e.g. a tail).
    m_body_location = { BodyPart::Hand, 0 };
    EntityId currently_wielded = subject->getWieldingIn(m_body_location);

    std::string bodypart_desc = subject->getBodypartDescription(m_body_location);

    // If it is us, or it is what is already being wielded, it means to unwield whatever is wielded.
    if ((object == subject) || (object == currently_wielded) || (object == EntityId::Mu()))
    {
      std::unique_ptr<Action> unwieldAction(NEW ActionUnwield(subject));
      subject->queueAction(std::move(unwieldAction));

      return StateResult::Failure();
    }
    else if (currently_wielded != EntityId::Mu())
    {
      putMsg(makeTr("YOU_MUST_UNWIELD_FIRST", { bodypart_desc }));
      return StateResult::Failure();
    }

    // Check that we have hands capable of wielding anything.
    if (subject->getBodypartNumber(BodyPart::Hand) == 0)
    {
      printMessageTry();
      putTr("YOU_HAVE_NO_GRASPING_LIMBS");
      return StateResult::Failure();
    }

    return StateResult::Success();
  }

  StateResult ActionWield::doBeginWorkNVI(AnyMap& params)
  {
    /// @todo Wielding should take time -- should not be instantaneously done here.
    auto subject = getSubject();
    auto object = getObject();

    printMessageBegin();

    // If we HAVE a new item, try to wield it.
    if (object->be_object_of(*this, subject))
    {
      /// @todo Figure out action time.
      return StateResult::Success();
    }

    return StateResult::Failure();
  }

  StateResult ActionWield::doFinishWorkNVI(AnyMap& params)
  {
    auto subject = getSubject();
    auto object = getObject();
    std::string bodypart_desc =
      subject->getBodypartDescription(m_body_location);

    subject->setWielded(object, m_body_location);
    putMsg(makeTr("YOU_ARE_NOW_WIELDING_THE_FOO",
                  { subject->getPossessiveString(bodypart_desc) }));

    return StateResult::Success();
  }

  StateResult ActionWield::doAbortWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }
} // end namespace
