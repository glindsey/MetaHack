#include "ActionAttack.h"

#include "components/ComponentManager.h"
#include "config/Strings.h"
#include "game/GameState.h"
#include "map/Map.h"
#include "map/MapFactory.h"
#include "maptile/MapTile.h"
#include "services/IMessageLog.h"
#include "services/Service.h"
#include "systems/Manager.h"
#include "systems/SystemNarrator.h"
#include "utilities/Shortcuts.h"

namespace Actions
{
  ActionAttack ActionAttack::prototype;
  ActionAttack::ActionAttack() : Action("ATTACK", ActionAttack::create_) {}
  ActionAttack::ActionAttack(EntityId subject) : Action(subject, "ATTACK") {}
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

  StateResult ActionAttack::doPreBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    auto subject = getSubject();
    auto location = COMPONENTS.position[subject].parent();
    auto new_direction = getTargetDirection();
    auto& narrator = systems.narrator();

    // Check if we're attacking ourself.
    if (new_direction == Direction::Self)
    {
      /// @todo Allow attacking yourself!
      putMsg(narrator.makeTr("YOU_TRY_TO_ATTACK_YOURSELF_HUMOROUS", arguments));
      return StateResult::Failure();
    }

    if (new_direction == Direction::Up)
    {
      /// @todo Write up/down attack code
      putMsg(narrator.makeTr("YOU_CANT_ATTACK_CEILING", arguments));
      return StateResult::Failure();
    }

    if (new_direction == Direction::Down)
    {
      /// @todo Write up/down attack code
      putMsg(narrator.makeTr("YOU_CANT_ATTACK_FLOOR", arguments));
      return StateResult::Failure();
    }

    return StateResult::Success();
  }

  StateResult ActionAttack::doBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    auto subject = getSubject();
    auto& position = COMPONENTS.position[subject];
    auto new_direction = getTargetDirection();
    auto& narrator = systems.narrator();

    bool success = false;
    unsigned int action_time = 0;

    // Figure out our target location.
    IntVec2 coords = position.coords();
    MapID map = position.map();
    IntVec2 offset = (IntVec2)new_direction;
    int x_new = coords.x + offset.x;
    int y_new = coords.y + offset.y;
    IntVec2 map_size = MAPS.get(map).getSize();

    // Check boundaries.
    if ((x_new < 0) || (y_new < 0) ||
      (x_new >= map_size.x) || (y_new >= map_size.y))
    {
      putMsg(narrator.makeTr("YOU_CANT_VERB_THERE", arguments));
      return StateResult::Failure();
    }

    auto& new_tile = MAPS.get(map).getTile({ x_new, y_new });
    EntityId new_floor = new_tile.getSpaceEntity();

    // See if the tile to move into contains another creature.
    auto object = COMPONENTS.inventory[new_floor].getEntity();
    setObject(object);
    if (object == EntityId::Void)
    {
      /// @todo Deal with attacking other stuff, MapTiles, etc.
      putMsg(narrator.makeTr("YOU_CANT_VERB_NOTHING", arguments));
      return StateResult::Failure();
    }

    bool reachable = COMPONENTS.position[subject].isAdjacentTo(object);
    /// @todo deal with DynamicEntities in your Inventory -- WTF do you do THEN?

    if (reachable)
    {
      /// @todo Write actual attack code here.
      putMsg(narrator.makeTr("ACTN_ATTACK_UNFINISHED", arguments));
    }

    return{ success, action_time };
  }

  StateResult ActionAttack::doFinishWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  StateResult ActionAttack::doAbortWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }
} // end namespace
