#pragma once

#include "stdafx.h"

#include "Thing.h"
#include "Renderable.h"

/// Abstract cLass representing a view of a Thing object.
class ThingView : public RenderableToTexture
{
public:
  /// Constructor.
  ThingView();

  /// Constructor.
  /// @param thing	ID of Thing object to associate with this view.
  explicit ThingView(ThingId thing_id);

  /// Destructor.
  virtual ~ThingView();

  /// Sets the current thing ID.
  /// If new thing ID is different from the current thing ID,
  /// this method may reinitialize cached render data.
  /// @param thing_id ID of the thing to view.
  void set_thing_id(ThingId thing_id);

  /// Gets the current thing ID.
  /// @return The ID of the thing being viewed.
  ThingId get_thing_id();

  /// @todo Method to set location to draw to on texture.

  /// Sets the location of this Thing on the target texture.
  /// If the new location is different from previous, this method may
  /// reinitialize cached render data.
  /// @param target_coords Target coords for the Thing's upper-left corner.
  void set_location(sf::Vector2f target_coords);

  /// Gets the location of this Thing on the target texture.
  /// @return Target coords for the Thing's upper-left corner.
  sf::Vector2f get_location();

  /// Sets the size of this Thing on the target texture.
  /// If the new size is different from previous, this method may
  /// reinitialize cached render data.
  /// @param target_coords Size of the Thing, in pixels.
  void set_size(sf::Vector2u target_size);

  /// Gets the size of this Thing on the target texture.
  /// @return Size of the Thing, in pixels.
  sf::Vector2u get_size();

  /// Update any cached render data associated with thing.
  /// @param thing	ID of the thing that is percieving the map.
  virtual void update_render_data(ThingId viewer) = 0;

protected:
  /// Get reference to Thing associated with this view.
  Thing& get_thing();

  /// Reinitialize cached Thing render data.
  virtual void reset_cached_render_data() = 0;

private:
  /// Thing ID associated with this view.
  ThingId m_thing_id;

  /// Target coordinates for the Thing.
  sf::Vector2f m_target_coords;

  /// Target size for the Thing, in pixels.
  sf::Vector2u m_target_size;
};