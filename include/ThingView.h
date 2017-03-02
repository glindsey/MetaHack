#pragma once

#include "stdafx.h"

#include "Thing.h"
#include "Observer.h"

/// Abstract class representing a view of a single Thing object.
class ThingView 
  : 
  public Observer
{
public:
  /// Constructor.
  explicit ThingView(Thing& thing);

  /// Destructor.
  virtual ~ThingView();

  /// Sets the location of this Thing on the target texture.
  /// If the new location is different from previous, this method may
  /// reinitialize cached render data.
  /// @param target_coords Target coords for the Thing's upper-left corner.
  void set_location(Vec2f target_coords);

  /// Gets the location of this Thing on the target texture.
  /// @return Target coords for the Thing's upper-left corner.
  Vec2f get_location();

  /// Sets the size of this Thing on the target texture.
  /// If the new size is different from previous, this method may
  /// reinitialize cached render data.
  /// @param target_coords Size of the Thing, in pixels.
  void set_size(Vec2u target_size);

  /// Gets the size of this Thing on the target texture.
  /// @return Size of the Thing, in pixels.
  Vec2u get_size();

protected:
  /// Get reference to Thing associated with this view.
  Thing& get_thing();

private:
  /// Reference to Thing associated with this view.
  Thing& m_thing;

  /// Target coordinates for the Thing.
  Vec2f m_target_coords;

  /// Target size for the Thing, in pixels.
  Vec2u m_target_size;
};