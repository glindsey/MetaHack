#include "stdafx.h"

#include "ActionWear.h"
#include "components/ComponentManager.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "systems/Manager.h"
#include "systems/SystemNarrator.h"
#include "utilities/Shortcuts.h"

#include "entity/Entity.h" // still needed for queueAction(), beObjectOf(), is_equippable_on()

namespace Actions
{
  ActionWear ActionWear::prototype;
  ActionWear::ActionWear() : Action("WEAR", ActionWear::create_) {}
  ActionWear::ActionWear(EntityId subject) : Action(subject, "WEAR") {}
  ActionWear::~ActionWear() {}

  ReasonBool ActionWear::subjectIsCapable(GameState const& gameState) const
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

  StateResult ActionWear::doPreBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    auto subject = getSubject();
    auto object = getObject();
    auto& narrator = systems.narrator();

    auto bodypart = object->is_equippable_on();

    if (bodypart == BodyPart::MemberCount)
    {
      putMsg(narrator.makeTr("THE_FOO_IS_NOT_VERBABLE", arguments));
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

  StateResult ActionWear::doBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
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

  StateResult ActionWear::doFinishWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    auto subject = getSubject();
    auto object = getObject();
    auto& narrator = systems.narrator();

    std::string bodypart_desc = narrator.getBodypartDescription(subject, m_bodyLocation);

    COMPONENTS.bodyparts[subject].wearEntity(object, m_bodyLocation);
    arguments["your_bodypart"] = narrator.getPossessiveString(subject, bodypart_desc);
    putMsg(narrator.makeTr("YOU_ARE_NOW_WEARING_THE_FOO", arguments));

    return StateResult::Success();
  }

  StateResult ActionWear::doAbortWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }
}
