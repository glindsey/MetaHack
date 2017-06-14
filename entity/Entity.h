#pragma once

#include "actions/Action.h"
#include "types/BodyPart.h"
#include "types/Direction.h"
#include "types/GameObject.h"
#include "types/Gender.h"
#include "lua/LuaObject.h"
#include "types/MapMemoryChunk.h"
#include "map/MapMemory.h"
#include "maptile/MapTile.h"
#include "Object.h"
#include "entity/EntityId.h"
#include "entity/EntityFactory.h"

// Forward declarations
namespace Components
{
  class Inventory;
  class Position;
}

class ComponentInventory;
class ComponentPosition;
class Entity;
class EntityId;
class MapTile;

/// Entity is any object in the game, animate or not.
/// @todo Still a LOT of cruft in this class that should be refactored out,
///       e.g. the wearing/wielding stuff.
class Entity
  :
  public GameObject
{
  friend class EntityFactory;

public:

  virtual ~Entity();

  virtual std::string getDisplayName() const override;

  /// Return a reference to this entity.
  EntityId getId() const;

protected:
  /// Named Constructor
  Entity(GameState& state, std::string category, EntityId ref);

  /// Clone Constructor
  Entity(Entity const& original, EntityId ref);

private:
  /// Reference to game state.
  GameState& m_state;

  /// Reference to this Entity.
  EntityId m_id;

  /// Outline color for walls when drawing on-screen.
  static Color const wall_outline_color_;
};
