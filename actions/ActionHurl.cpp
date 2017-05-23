#include "stdafx.h"

#include "ActionHurl.h"
#include "components/ComponentManager.h"
#include "game/GameState.h"
#include "services/IMessageLog.h"
#include "Service.h"
#include "systems/SystemManager.h"
#include "systems/SystemSpacialRelationships.h"

#include "entity/Entity.h"
#include "entity/EntityId.h"


namespace Actions
{
  ActionHurl ActionHurl::prototype;
  ActionHurl::ActionHurl() : Action("hurl", "THROW", ActionHurl::create_) {}
  ActionHurl::ActionHurl(EntityId subject) : Action(subject, "hurl", "THROW") {}
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

  StateResult ActionHurl::doPreBeginWorkNVI(GameState& gameState, SystemManager& systems)
  {
    std::string message;
    auto subject = getSubject();
    auto object = getObject();
    auto new_direction = getTargetDirection();

    return StateResult::Success();
  }

  StateResult ActionHurl::doBeginWorkNVI(GameState& gameState, SystemManager& systems)
  {
    auto result = StateResult::Failure();
    std::string message;
    auto subject = getSubject();
    auto object = getObject();
    auto direction = getTargetDirection();
    EntityId new_location = COMPONENTS.position[subject].parent();

    if (object->beObjectOf(*this, subject, direction))
    {
      if (SYSTEMS.spacial().moveEntityInto(object, new_location))
      {
        printMessageDo();

        /// @todo When throwing, set Entity's direction and velocity
        /// @todo Figure out action time.
        result = StateResult::Success();
      }
      else
      {
        putTr("YOU_CANT_VERB_FOO_UNKNOWN");

        CLOG(WARNING, "Action") << "Could not throw Entity " << object <<
          " even though beObjectOf returned Success";
      }
    }

    return result;
  }

  StateResult ActionHurl::doFinishWorkNVI(GameState& gameState, SystemManager& systems)
  {
    return StateResult::Success();
  }

  StateResult ActionHurl::doAbortWorkNVI(GameState& gameState, SystemManager& systems)
  {
    return StateResult::Success();
  }
} // end namespace
