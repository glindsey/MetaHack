#pragma once

#include "entity/Entity.h"
#include "Object.h"

// Forward declarations
namespace Systems
{
  class Lighting;
}

/// Abstract class representing a view of a single Entity object.
class EntityView 
  : 
  public Object
{
  friend class NullGraphicViews;
  friend class Standard2DGraphicViews;

public:
  virtual ~EntityView();

  /// Sets the location of this Entity on the target texture.
  /// If the new location is different from previous, this method may
  /// reinitialize cached render data.
  /// @param target_coords Target coords for the Entity's upper-left corner.
  void setLocation(RealVec2 target_coords);

  /// Gets the location of this Entity on the target texture.
  /// @return Target coords for the Entity's upper-left corner.
  RealVec2 getLocation();

  /// Sets the size of this Entity on the target texture.
  /// If the new size is different from previous, this method may
  /// reinitialize cached render data.
  /// @param target_coords Size of the Entity, in pixels.
  void setSize(RealVec2 target_size);

  /// Gets the size of this Entity on the target texture.
  /// @return Size of the Entity, in pixels.
  RealVec2 getSize();

  virtual void draw(sf::RenderTarget& target,
                    Systems::Lighting* lighting,
                    bool use_smoothing,
                    int frame) = 0;

  virtual std::string getViewName() = 0;

protected:
  /// Constructor.
  explicit EntityView(Entity& entity);

  /// Get reference to Entity associated with this view.
  Entity& getEntity();

  Entity const& getEntity() const;

private:
  /// Reference to Entity associated with this view.
  Entity& m_entity;

  /// Target coordinates for the Entity.
  RealVec2 m_target_coords;

  /// Target size for the Entity, in pixels.
  RealVec2 m_target_size;
};