#include "stdafx.h"

#include "ActionAttack.h"
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

  StateResult ActionAttack::do_prebegin_work_(AnyMap& params)
  {
    std::string message;

    auto subject = get_subject();
    auto location = subject->getLocation();
    auto new_direction = get_target_direction();

    // Check if we're attacking ourself.
    if (new_direction == Direction::Self)
    {
      /// @todo Allow attacking yourself!
      message += maketr("YOU_TRY_TO_ATTACK_YOURSELF_HUMOROUS");
      Service<IMessageLog>::get().add(message);
      return StateResult::Failure();
    }

    if (new_direction == Direction::Up)
    {
      /// @todo Write up/down attack code
      message = tr("YOU_CANT_ATTACK_CEILING");
      Service<IMessageLog>::get().add(message);
      return StateResult::Failure();
    }

    if (new_direction == Direction::Down)
    {
      /// @todo Write up/down attack code
      message = tr("YOU_CANT_ATTACK_FLOOR");
      Service<IMessageLog>::get().add(message);
      return StateResult::Failure();
    }

    return StateResult::Success();
  }

  StateResult ActionAttack::do_begin_work_(AnyMap& params)
  {
    std::string message;

    auto subject = get_subject();
    auto location = subject->getLocation();
    MapTile* current_tile = subject->get_maptile();
    auto new_direction = get_target_direction();

    bool success = false;
    unsigned int action_time = 0;

    // Figure out our target location.
    IntVec2 coords = current_tile->get_coords();
    IntVec2 offset = (IntVec2)new_direction;
    int x_new = coords.x + offset.x;
    int y_new = coords.y + offset.y;
    Map& current_map = GAME.get_maps().get(subject->get_map_id());
    IntVec2 map_size = current_map.getSize();

    // Check boundaries.
    if ((x_new < 0) || (y_new < 0) ||
      (x_new >= map_size.x) || (y_new >= map_size.y))
    {
      message += maketr("YOU_CANT_VERB_THERE");
      Service<IMessageLog>::get().add(message);
      return StateResult::Failure();
    }

    auto& new_tile = current_map.get_tile({ x_new, y_new });
    EntityId new_floor = new_tile.get_tile_contents();

    // See if the tile to move into contains another creature.
    auto object = new_floor->get_inventory().getEntity();
    set_object(object);
    if (object == EntityId::Mu())
    {
      /// @todo Deal with attacking other stuff, MapTiles, etc.
      message = maketr("YOU_CANT_VERB_NOTHING");
      Service<IMessageLog>::get().add(message);
      return StateResult::Failure();
    }

    bool reachable = subject->is_adjacent_to(object);
    /// @todo deal with DynamicEntities in your Inventory -- WTF do you do THEN?

    if (reachable)
    {
      /// @todo Write actual attack code here.
      message = maketr("ACTN_ATTACK_UNFINISHED");
      Service<IMessageLog>::get().add(message);
    }

    return{ success, action_time };
  }

  StateResult ActionAttack::do_finish_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionAttack::do_abort_work_(AnyMap& params)
  {
    return StateResult::Success();
  }
} // end namespace
