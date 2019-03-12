#include "ActionClose.h"

#include "components/ComponentManager.h"
#include "config/Strings.h"
#include "services/IMessageLog.h"
#include "services/Service.h"
#include "utilities/Shortcuts.h"

namespace Actions
{
  ActionClose ActionClose::prototype;
  ActionClose::ActionClose() : Action("CLOSE", ActionClose::create_) {}
  ActionClose::ActionClose(EntityId subject) : Action(subject, "CLOSE") {}
  ActionClose::~ActionClose() {}

  ReasonBool ActionClose::subjectIsCapable(GameState const& gameState) const
  {
    auto subject = getSubject();
    bool isSapient = COMPONENTS.sapience.existsFor(subject);
    bool canGrasp = COMPONENTS.bodyparts.existsFor(subject) && COMPONENTS.bodyparts[subject].hasPrehensileBodyPart();

    if (!isSapient) return { false, "YOU_ARE_NOT_SAPIENT" }; ///< @todo Add translation key
    if (!canGrasp) return { false, "YOU_HAVE_NO_GRASPING_BODYPARTS" }; ///< @todo Add translation key

    return { true, "" };
  }

  std::unordered_set<Trait> const & ActionClose::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObject,
      Trait::CanBeSubjectVerbDirection
    };

    return traits;
  }

  StateResult ActionClose::doPreBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  StateResult ActionClose::doBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    bool success = false;
    unsigned int action_time = 0;

    putMsg(tr("ACTN_NOT_IMPLEMENTED"));

#if 0
    if (entity != EntityId::Void)
    {
      success = actor->do_close(entity, action_time);
    }
#endif

    return{ success, action_time };
  }

  StateResult ActionClose::doFinishWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  StateResult ActionClose::doAbortWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }
}

