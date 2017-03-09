#pragma once

#include "stdafx.h"

#include "Entity.h"
#include "Observer.h"

/// Abstract class representing a view of a single Entity object.
class EntityView 
  : 
  public Observer
{
  friend class NullGraphicViews;
  friend class Standard2DGraphicViews;

public:
  /// Destructor.
  virtual ~EntityView();

  /// Sets the location of this Entity on the target texture.
  /// If the new location is different from previous, this method may
  /// reinitialize cached render data.
  /// @param target_coords Target coords for the Entity's upper-left corner.
  void set_location(Vec2f target_coords);

  /// Gets the location of this Entity on the target texture.
  /// @return Target coords for the Entity's upper-left corner.
  Vec2f get_location();

  /// Sets the size of this Entity on the target texture.
  /// If the new size is different from previous, this method may
  /// reinitialize cached render data.
  /// @param target_coords Size of the Entity, in pixels.
  void setSize(Vec2f target_size);

  /// Gets the size of this Entity on the target texture.
  /// @return Size of the Entity, in pixels.
  Vec2f getSize();

  virtual void draw(sf::RenderTarget& target,
                    bool use_lighting,
                    bool use_smoothing,
                    int frame) = 0;

  virtual std::string get_view_name() = 0;

protected:
  /// Constructor.
  explicit EntityView(Entity& entity);

  /// Get reference to Entity associated with this view.
  Entity& get_entity();

  Entity const& get_entity() const;

private:
  /// Reference to Entity associated with this view.
  Entity& m_entity;

  /// Target coordinates for the Entity.
  Vec2f m_target_coords;

  /// Target size for the Entity, in pixels.
  Vec2f m_target_size;
};