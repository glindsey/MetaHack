#include "stdafx.h"

#include "ActionAttack.h"
#include "GameState.h"
#include "IMessageLog.h"
#include "IStringDictionary.h"
#include "Map.h"
#include "MapFactory.h"
#include "Service.h"
#include "Entity.h"
#include "EntityId.h"

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
      Trait::CanBeSubjectVerbDirection
    };

    return traits;
  }

  StateResult ActionAttack::do_prebegin_work_(AnyMap& params)
  {
    std::string message;

    auto subject = get_subject();
    auto location = subject->getLocation();
    auto new_direction = get_target_direction();

    if (!IS_PLAYER)
    {
      print_message_try_();
      message = "But ";
    }
    else
    {
      message = "";
    }

    // Make sure we CAN attack.
    if (!subject->get_modified_property<bool>("can_attack", false))
    {
      message += maketr("ACTION_CANT_ATTACK");
      Service<IMessageLog>::get().add(message);
      return StateResult::Failure();
    }

    // Make sure we can move RIGHT NOW.
    if (!subject->can_currently_move())
    {
      message += maketr("ACTION_CANT_MOVE_NOW");
      Service<IMessageLog>::get().add(message);
      return StateResult::Failure();
    }

    // Check if we're attacking ourself.
    if (new_direction == Direction::Self)
    {
      /// @todo Allow attacking yourself!
      message = maketr("ACTION_WONT_ATTACK_SELF");
      Service<IMessageLog>::get().add(message);
      return StateResult::Failure();
    }

    // Make sure we're not confined inside another entity.
    /// @todo Allow for attacking when swallowed!
    if (subject->is_inside_another_thing())
    {
      message += maketr("ACTION_INSIDE_OBJECT",
      { location->get_identifying_string(ArticleChoice::Indefinite) });

      Service<IMessageLog>::get().add(message);
      return StateResult::Failure();
    }

    if (new_direction == Direction::Up)
    {
      /// @todo Write up/down attack code
      message = tr("ACTION_CANT_ATTACK_CEILING");
      Service<IMessageLog>::get().add(message);
      return StateResult::Failure();
    }

    if (new_direction == Direction::Down)
    {
      /// @todo Write up/down attack code
      message = tr("ACTION_CANT_ATTACK_FLOOR");
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
    Vec2i coords = current_tile->get_coords();
    Vec2i offset = (Vec2i)new_direction;
    int x_new = coords.x + offset.x;
    int y_new = coords.y + offset.y;
    Map& current_map = GAME.get_maps().get(subject->get_map_id());
    Vec2i map_size = current_map.getSize();

    // Check boundaries.
    if ((x_new < 0) || (y_new < 0) ||
      (x_new >= map_size.x) || (y_new >= map_size.y))
    {
      message += maketr("ACTION_CANT_VERB_THERE");
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
      message = maketr("ACTION_CANT_VERB_NOTHING");
      Service<IMessageLog>::get().add(message);
      return StateResult::Failure();
    }

    bool reachable = subject->is_adjacent_to(object);
    /// @todo deal with DynamicEntities in your Inventory -- WTF do you do THEN?

    if (reachable)
    {
      /// @todo Write actual attack code here.
      message = maketr("ACTION_ATTACK_UNFINISHED");
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
