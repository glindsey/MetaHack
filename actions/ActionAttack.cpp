#include "stdafx.h"

#include "ActionAttack.h"
#include "components/ComponentManager.h"
#include "game/GameState.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "map/Map.h"
#include "map/MapFactory.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

namespace Actions
{
  ActionAttack ActionAttack::prototype;
  ActionAttack::ActionAttack() : Action("attack", "ATTACK", ActionAttack::create_) {}
  ActionAttack::ActionAttack(EntityId subject) : Action(subject, "attack", "ATTACK") {}
  ActionAttack::~ActionAttack() {}

  std::unordered_set<Trait> const & ActionAttack::getTraits() const
  {
    static std::unordered_set<Trait> traits = 
    {
      Trait::CanBeSubjectVerbObject,
      Trait::CanBeSubjectVerbDirection,
      Trait::SubjectCanNotBeInsideAnotherObject     ///< @todo Allow for attacking when swallowed!
    };

    return traits;
  }

  StateResult ActionAttack::doPreBeginWorkNVI(AnyMap& params)
  {
    auto subject = getSubject();
    auto location = subject->getLocation();
    auto new_direction = getTargetDirection();

    // Check if we're attacking ourself.
    if (new_direction == Direction::Self)
    {
      /// @todo Allow attacking yourself!
      putTr("YOU_TRY_TO_ATTACK_YOURSELF_HUMOROUS");
      return StateResult::Failure();
    }

    if (new_direction == Direction::Up)
    {
      /// @todo Write up/down attack code
      putTr("YOU_CANT_ATTACK_CEILING");
      return StateResult::Failure();
    }

    if (new_direction == Direction::Down)
    {
      /// @todo Write up/down attack code
      putTr("YOU_CANT_ATTACK_FLOOR");
      return StateResult::Failure();
    }

    return StateResult::Success();
  }

  StateResult ActionAttack::doBeginWorkNVI(AnyMap& params)
  {
    auto subject = getSubject();
    auto& position = COMPONENTS.position[subject];
    auto new_direction = getTargetDirection();

    bool success = false;
    unsigned int action_time = 0;

    // Figure out our target location.
    IntVec2 coords = position.coords();
    IntVec2 offset = (IntVec2)new_direction;
    int x_new = coords.x + offset.x;
    int y_new = coords.y + offset.y;
    Map& current_map = GAME.maps().get(subject->getMapId());
    IntVec2 map_size = current_map.getSize();

    // Check boundaries.
    if ((x_new < 0) || (y_new < 0) ||
      (x_new >= map_size.x) || (y_new >= map_size.y))
    {
      putTr("YOU_CANT_VERB_THERE");
      return StateResult::Failure();
    }

    auto& new_tile = current_map.getTile({ x_new, y_new });
    EntityId new_floor = new_tile.getTileContents();

    // See if the tile to move into contains another creature.
    auto object = new_floor->getInventory().getEntity();
    setObject(object);
    if (object == EntityId::Mu())
    {
      /// @todo Deal with attacking other stuff, MapTiles, etc.
      putTr("YOU_CANT_VERB_NOTHING");
      return StateResult::Failure();
    }

    bool reachable = subject->isAdjacentTo(object);
    /// @todo deal with DynamicEntities in your Inventory -- WTF do you do THEN?

    if (reachable)
    {
      /// @todo Write actual attack code here.
      putTr("ACTN_ATTACK_UNFINISHED");
    }

    return{ success, action_time };
  }

  StateResult ActionAttack::doFinishWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionAttack::doAbortWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }
} // end namespace
