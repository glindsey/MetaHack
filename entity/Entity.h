#pragma once

#include "stdafx.h"

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
#include "entity/EntityPool.h"

// Forward declarations
class AIStrategy;
class ComponentInventory;
class ComponentPosition;
class DynamicEntity;
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
  friend class AIStrategy;
  friend class EntityPool;

public:

  virtual ~Entity();

  virtual std::string getDisplayName() const override;

  /// Queue an action for this DynamicEntity to perform.
  /// @param action The Action to queue. The Action is MOVED when queued,
  ///               e.g. pAction will be `nullptr` after queuing.
  void queueAction(std::unique_ptr<Actions::Action> action);

  /// Queue an action for this DynamicEntity to perform.
  /// @param p_action The Action to queue. The queue takes ownership of the
  ///                 pointer.
  void queueAction(Actions::Action* p_action);

  /// Return whether there is an action pending for this DynamicEntity.
  bool actionIsPending() const;

  /// Return whether there is an action currently in progress for this DynamicEntity.
  bool actionIsInProgress();

  /// Clear all pending actions in this Entity's queue.
  void clearAllPendingActions();
  
  /// Return a reference to this entity.
  EntityId getId() const;

  /// Spill the contents of this Entity out into the location of the Entity.
  //void spill();

  /// Attempt to destroy this Entity.
  void destroy();

  /// Return the body part this entity is equippable on.
  /// If entity is not equippable, return BodyPart::Count.
  BodyPart is_equippable_on() const;

  /// Process voluntary actions of this Entity and its inventory for a single tick.
  bool processActions();

  /// Perform an action when this entity dies.
  /// @return If this function returns false, the death is avoided.
  /// @warning The function must reset whatever caused the death in the
  ///          first place, or the Entity will just immediately die again
  ///          on the next call to process()!
  bool perform_action_die();

  /// Perform an action when this entity collides with another entity.
  void perform_action_collide_with(EntityId entity);

  /// Perform an action when this entity collides with a wall.
  void perform_action_collide_with_wall(Direction d, std::string tile_type);

  /// Perform the effects of being the subject of a particular action.
  /// This is typically called on intransitive verbs (ones that do not take
  /// an object); "die" is a good example. You can't "die something" (though
  /// you can "dye something" but that's beside the point), so this would be
  /// an appropriate method to call.
  /// 
  /// @param action   The action to be the target of.
  /// @return Bool indicating whether the action succeeded.
  bool do_(Actions::Action& action);

  /// Perform the effects of being a object of a particular action.
  /// @param action   The action to be the target of.
  /// @param subject  The subject performing the action.
  /// @return Bool indicating whether the action succeeded.
  bool beObjectOf(Actions::Action& action, EntityId subject);

  /// Perform the effects of being a object of an action with a target.
  /// @param action   The action to be the target of.
  /// @param subject  The subject performing the action.
  /// @param target   The target of the action.
  /// @return Bool indicating whether the action succeeded.
  bool beObjectOf(Actions::Action& action, EntityId subject, EntityId target);

  /// Perform the effects of being a object of an action with a direction.
  /// @param action     The action to be the target of.
  /// @param subject    The subject performing the action.
  /// @param direction  The direction of the action.
  /// @return Bool indicating whether the action succeeded.
  bool beObjectOf(Actions::Action& action, EntityId subject, Direction direction);

  /// Perform an action when this entity is hit by an attack.
  bool be_hurt_by(EntityId subject);

  /// Perform an action when this entity is used to hit a target.
  /// This action executes when the entity is wielded by an entity, and an
  /// attack successfully hits its target.  It is a side-effect in addition
  /// to the damage done by the attack action.
  bool be_used_to_attack(EntityId subject, EntityId target);

  /// Returns whether the Entity can merge with another Entity.
  /// Calls an overridden subclass function.
  bool can_merge_with(EntityId other) const;

  /// Syntactic sugar for calling call_lua_function().
  json call_lua_function(std::string function_name,
                         json const& args,
                         json const& default_result);

  json call_lua_function(std::string function_name,
                         json const& args,
                         json const& default_result) const;

  /// Get a const reference to this entity's category data.
  json const& getCategoryData() const;

protected:
  /// Named Constructor
  Entity(GameState& state, std::string category, EntityId ref);

  /// Clone Constructor
  Entity(Entity const& original, EntityId ref);

  /// Initializer; called by all constructors.
  void initialize();

  /// Process this Entity's voluntary actions for a single tick.
  /// The function returns false to indicate to its parent that it no longer
  /// exists and should be deleted.
  /// @return true if the Entity continues to exist after the tick;
  ///         false if the Entity ceases to exist.
  virtual bool processOwnActions_();

private:
  /// Reference to game state.
  GameState& m_state;

  /// Reference to this Entity.
  EntityId m_id;

  /// Outline color for walls when drawing on-screen.
  static Color const wall_outline_color_;
};
