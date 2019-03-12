#include "ActionOpen.h"

#include "components/ComponentManager.h"
#include "config/Strings.h"
#include "services/IMessageLog.h"
#include "services/Service.h"
#include "systems/Manager.h"
#include "systems/SystemNarrator.h"
#include "utilities/Shortcuts.h"

namespace Actions
{
  ActionOpen ActionOpen::prototype;
  ActionOpen::ActionOpen() : Action("OPEN", ActionOpen::create_) {}
  ActionOpen::ActionOpen(EntityId subject) : Action(subject, "OPEN") {}
  ActionOpen::~ActionOpen() {}

  ReasonBool ActionOpen::subjectIsCapable(GameState const& gameState) const
  {
    auto& components = gameState.components();
    auto subject = getSubject();
    bool isSapient = components.sapience.existsFor(subject);
    bool canGrasp = components.bodyparts.existsFor(subject) && components.bodyparts.of(subject).hasPrehensileBodyPart();

    if (!isSapient) return { false, "YOU_ARE_NOT_SAPIENT" }; ///< @todo Add translation key
    if (!canGrasp) return { false, "YOU_HAVE_NO_GRASPING_BODYPARTS" }; ///< @todo Add translation key

    return { true, "" };
  }

  std::unordered_set<Trait> const & ActionOpen::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObject,
      Trait::CanBeSubjectVerbDirection
    };

    return traits;
  }

  StateResult ActionOpen::doPreBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  StateResult ActionOpen::doBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    bool success = false;
    unsigned int action_time = 0;
    putMsg(tr("ACTN_NOT_IMPLEMENTED"));

#if 0
    if (entity != EntityId::Void)
    {
      success = actor->do_open(entity, action_time);
    }
#endif

    return{ success, action_time };
  }

  StateResult ActionOpen::doFinishWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  StateResult ActionOpen::doAbortWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }
} // end namespace

