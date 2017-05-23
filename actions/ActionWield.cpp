#include "stdafx.h"

#include "ActionWield.h"

#include "ActionUnwield.h"
#include "components/ComponentManager.h"
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

  ReasonBool ActionWield::subjectIsCapable(GameState const& gameState) const
  {
    auto subject = getSubject();
    bool isSapient = COMPONENTS.sapience.existsFor(subject);
    bool canGrasp = COMPONENTS.bodyparts.existsFor(subject) && COMPONENTS.bodyparts[subject].hasPrehensileBodyPart();

    if (!isSapient) return { false, "YOU_ARE_NOT_SAPIENT" }; ///< @todo Add translation key
    if (!canGrasp) return { false, "YOU_HAVE_NO_GRASPING_BODYPARTS" }; ///< @todo Add translation key

    return { true, "" };
  }

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

  StateResult ActionWield::doPreBeginWorkNVI(GameState& gameState, SystemManager& systems)
  {
    std::string message;
    auto subject = getSubject();
    auto object = getObject();

    /// @todo Support wielding in other prehensile limb(s). This will also include
    ///       shifting an already-wielded weapon to another hand.
    m_bodyLocation = { BodyPart::Hand, 0 };
    EntityId currentlyWielded = COMPONENTS.bodyparts[subject].getWieldedEntity(m_bodyLocation);

    std::string bodypartDesc = subject->getBodypartDescription(m_bodyLocation);

    // If it is us, or it is what is already being wielded, it means to unwield whatever is wielded.
    if ((object == subject) || (object == currentlyWielded) || (object == EntityId::Mu()))
    {
      std::unique_ptr<Action> unwieldAction(NEW ActionUnwield(subject));
      subject->queueAction(std::move(unwieldAction));

      return StateResult::Failure();
    }
    else if (currentlyWielded != EntityId::Mu())
    {
      putMsg(makeTr("YOU_MUST_UNWIELD_FIRST", { bodypartDesc }));
      return StateResult::Failure();
    }

    return StateResult::Success();
  }

  StateResult ActionWield::doBeginWorkNVI(GameState& gameState, SystemManager& systems)
  {
    /// @todo Wielding should take time -- should not be instantaneously done here.
    auto subject = getSubject();
    auto object = getObject();

    printMessageBegin();

    // If we HAVE a new item, try to wield it.
    if (object->beObjectOf(*this, subject))
    {
      /// @todo Figure out action time.
      return StateResult::Success();
    }

    return StateResult::Failure();
  }

  StateResult ActionWield::doFinishWorkNVI(GameState& gameState, SystemManager& systems)
  {
    auto subject = getSubject();
    auto object = getObject();
    std::string bodypart_desc = subject->getBodypartDescription(m_bodyLocation);

    COMPONENTS.bodyparts[subject].wieldEntity(object, m_bodyLocation);
    putMsg(makeTr("YOU_ARE_NOW_WIELDING_THE_FOO",
                  { subject->getPossessiveString(bodypart_desc) }));

    return StateResult::Success();
  }

  StateResult ActionWield::doAbortWorkNVI(GameState& gameState, SystemManager& systems)
  {
    return StateResult::Success();
  }
} // end namespace
