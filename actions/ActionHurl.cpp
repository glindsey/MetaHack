#include "ActionHurl.h"

#include "components/ComponentManager.h"
#include "game/GameState.h"
#include "lua/LuaObject.h"
#include "systems/Manager.h"
#include "systems/SystemNarrator.h"
#include "systems/SystemGeometry.h"
#include "utilities/Shortcuts.h"

namespace Actions
{
  ActionHurl ActionHurl::prototype;
  ActionHurl::ActionHurl() : Action("THROW", ActionHurl::create_) {}
  ActionHurl::ActionHurl(EntityId subject) : Action(subject, "THROW") {}
  ActionHurl::~ActionHurl() {}

  std::unordered_set<Trait> const & ActionHurl::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObjectPrepositionDirection,
      Trait::ObjectMustNotBeWorn,
      Trait::ObjectMustBeInInventory
    };

    return traits;
  }

  StateResult ActionHurl::doPreBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    std::string message;
    auto subject = getSubject();
    auto object = getObject();
    auto new_direction = getTargetDirection();

    return StateResult::Success();
  }

  StateResult ActionHurl::doBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    auto& components = gameState.components();
    auto& lua = gameState.lua();
    auto& narrator = systems.narrator();
    auto result = StateResult::Failure();
    std::string message;
    auto subject = getSubject();
    auto object = getObject();
    auto direction = getTargetDirection();
    EntityId new_location = components.position.existsFor(subject) ? components.position.of(subject).parent() : EntityId::Void;

    if (lua.doSubjectActionObjectDirection(subject, *this, object, direction))
    {
      if (systems.geometry().moveEntityInto(object, new_location))
      {
        printMessageDo(systems, arguments);

        /// @todo When throwing, set Entity's direction and velocity
        /// @todo Figure out action time.
        result = StateResult::Success();
      }
      else
      {
        putMsg(narrator.makeTr("YOU_CANT_VERB_FOO_UNKNOWN", arguments));

        CLOG(WARNING, "Action") << "Could not throw Entity " << object <<
          " even though beObjectOf returned Success";
      }
    }

    return result;
  }

  StateResult ActionHurl::doFinishWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  StateResult ActionHurl::doAbortWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }
} // end namespace
