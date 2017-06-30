#include "stdafx.h"

#include "ActionWield.h"

#include "ActionUnwield.h"
#include "components/ComponentManager.h"
#include "lua/LuaObject.h"
#include "services/IMessageLog.h"
#include "services/IStrings.h"
#include "services/Service.h"
#include "systems/Manager.h"
#include "systems/SystemDirector.h"
#include "systems/SystemNarrator.h"
#include "utilities/Shortcuts.h"

namespace Actions
{
  ActionWield ActionWield::prototype;
  ActionWield::ActionWield() : Action("WIELD", ActionWield::create_) {}
  ActionWield::ActionWield(EntityId subject) : Action(subject, "WIELD") {}
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

  StateResult ActionWield::doPreBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    std::string message;
    auto subject = getSubject();
    auto object = getObject();
    auto& components = gameState.components();
    auto& narrator = systems.narrator();

    /// @todo Support wielding in other prehensile limb(s). This will also include
    ///       shifting an already-wielded weapon to another hand.
    m_bodyLocation = { BodyPart::Hand, 0 };
    EntityId currentlyWielded = components.bodyparts[subject].getWieldedEntity(m_bodyLocation);

    std::string bodypartDesc = narrator.getBodypartDescription(subject, m_bodyLocation);

    // If it is us, or it is what is already being wielded, it means to unwield whatever is wielded.
    if ((object == subject) || (object == currentlyWielded) || (object == EntityId::Void))
    {
      std::unique_ptr<Action> unwieldAction(NEW ActionUnwield(subject));
      systems.director().queueEntityAction(subject, std::move(unwieldAction));

      return StateResult::Failure();
    }
    else if (currentlyWielded != EntityId::Void)
    {
      arguments["bodypart"] = bodypartDesc;
      putMsg(narrator.makeTr("YOU_MUST_UNWIELD_FIRST", arguments));
      return StateResult::Failure();
    }

    return StateResult::Success();
  }

  StateResult ActionWield::doBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    /// @todo Wielding should take time -- should not be instantaneously done here.
    auto& lua = gameState.lua();
    auto subject = getSubject();
    auto object = getObject();

    printMessageBegin(systems, arguments);

    // If we HAVE a new item, try to wield it.
    if (lua.doSubjectActionObject(subject, *this, object))
    {
      /// @todo Figure out action time.
      return StateResult::Success();
    }

    return StateResult::Failure();
  }

  StateResult ActionWield::doFinishWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    auto subject = getSubject();
    auto object = getObject();
    auto& components = gameState.components();
    auto& narrator = systems.narrator();

    std::string bodypart_desc = narrator.getBodypartDescription(subject, m_bodyLocation);

    COMPONENTS.bodyparts[subject].wieldEntity(object, m_bodyLocation);
    arguments["your_bodypart"] = narrator.getPossessiveString(subject, bodypart_desc);
    putMsg(narrator.makeTr("YOU_ARE_NOW_WIELDING_THE_FOO", arguments));

    return StateResult::Success();
  }

  StateResult ActionWield::doAbortWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }
} // end namespace
