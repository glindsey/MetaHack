#include "stdafx.h"

#include "ActionWear.h"
#include "components/ComponentManager.h"
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

  ReasonBool ActionWear::subjectIsCapable(GameState& gameState) const
  {
    auto subject = getSubject();
    bool isSapient = COMPONENTS.sapience.existsFor(subject);
    bool canGrasp = COMPONENTS.bodyparts.existsFor(subject) && COMPONENTS.bodyparts[subject].hasPrehensileBodyPart();

    if (!isSapient) return { false, "YOU_ARE_NOT_SAPIENT" }; ///< @todo Add translation key
    if (!canGrasp) return { false, "YOU_HAVE_NO_GRASPING_BODYPARTS" }; ///< @todo Add translation key

    return { true, "" };
  }

  std::unordered_set<Trait> const & ActionWear::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObjectPrepositionBodypart,
      Trait::ObjectMustBeInInventory
    };

    return traits;
  }

  StateResult ActionWear::doPreBeginWorkNVI(GameState& gameState, AnyMap& params)
  {
    auto subject = getSubject();
    auto object = getObject();

    auto bodypart = object->is_equippable_on();

    if (bodypart == BodyPart::MemberCount)
    {
      putTr("THE_FOO_IS_NOT_VERBABLE");
      return StateResult::Failure();
    }
    else
    {
      /// @todo Check that entity has free body part(s) to equip item on.
      m_bodyLocation.part = bodypart;
      m_bodyLocation.number = 0;
    }

    return StateResult::Success();
  }

  StateResult ActionWear::doBeginWorkNVI(GameState& gameState, AnyMap& params)
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

  StateResult ActionWear::doFinishWorkNVI(GameState& gameState, AnyMap& params)
  {
    auto subject = getSubject();
    auto object = getObject();

    std::string bodypart_desc = subject->getBodypartDescription(m_bodyLocation);

    COMPONENTS.bodyparts[subject].wearEntity(object, m_bodyLocation);
    putMsg(makeTr("YOU_ARE_NOW_WEARING_THE_FOO", { subject->getPossessiveString(bodypart_desc) }));

    return StateResult::Success();
  }

  StateResult ActionWear::doAbortWorkNVI(GameState& gameState, AnyMap& params)
  {
    return StateResult::Success();
  }
}
