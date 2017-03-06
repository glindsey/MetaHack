#pragma once

#include "stdafx.h"

#include "Map.h"
#include "Observer.h"
#include "Renderable.h"

// Forward declarations
class EntityId;

/// Abstract class representing a view of a Map object.
class MapView 
  : 
  public Observer,
  public RenderableToTexture
{
public:
  /// Constructor.
  /// @param map	Reference to Map object to associate with this view.
  explicit MapView(Map& map);

  /// Destructor.
  virtual ~MapView();

  /// Sets map view information.
  /// @param center The place to center the rendered map on.
  /// @param zoom_level The zoom level.
  /// @todo Not sure this actually belongs here. It seems more specific to
  ///       AppStateGameMode itself; or possibly the interface is fine but
  ///       the implementation should go into MapStandard2DView.
  void set_view(sf::RenderTarget& target,
                Vec2f center,
                float zoom_level);

  /// Update any cached render data associated with map tiles.
  /// @param entity	ID of the entity that is percieving the map.
  virtual void update_tiles(EntityId entity) = 0;

  /// Update any cached render data associated with map entities.
  /// @param entity	ID of the entity that is percieving the map.
  /// @param frame	Current animation frame number.
  virtual void update_things(EntityId entity, int frame) = 0;


  virtual void draw_highlight(sf::RenderTarget& target,
                              Vec2f location,
                              sf::Color fgColor,
                              sf::Color bgColor,
                              int frame) = 0;

protected:
  /// Get reference to Map associated with this view.
  Map& get_map();

  virtual void notifyOfEvent_(Observable& observed, Event event) = 0;

private:
  /// Map associated with this view.
  Map& m_map;
};