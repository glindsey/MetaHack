#pragma once

#include "gui/GUIObject.h"
#include "map/Map.h"

// Forward declarations
class EntityId;

namespace Systems
{
  class Lighting;
}

/// Abstract class representing a view of a Map object.
class MapView 
  : 
  public metagui::GUIObject
{
public:
  /// Constructor.
  /// @param map	Reference to Map object to associate with this view.
  MapView(metagui::Desktop& desktop,
          std::string name,
          Map& map,
          UintVec2 size);

  /// Destructor.
  virtual ~MapView();

  /// Sets map view information.
  /// @param center The place to center the rendered map on.
  /// @param zoom_level The zoom level.
  /// @todo Not sure this actually belongs here. It seems more specific to
  ///       AppStateGameMode itself; or possibly the interface is fine but
  ///       the implementation should go into MapStandard2DView.
  void setView(sf::RenderTarget& target,
                RealVec2 center,
                float zoom_level);

  /// Render the map to a texture.
  /// @todo Get rid of this in favor of the metagui::GUIObject render methods.
  virtual bool renderMap(sf::RenderTexture& texture, int frame) = 0;

  /// Update any cached render data associated with map tiles.
  /// @param entity	ID of the entity that is percieving the map.
  virtual void updateTiles(EntityId entity, Systems::Lighting& lighting) = 0;

  /// Update any cached render data associated with map entities.
  /// @param entity	ID of the entity that is percieving the map.
  /// @param frame	Current animation frame number.
  virtual void updateEntities(EntityId entity, Systems::Lighting& lighting, int frame) = 0;


  virtual void drawHighlight(sf::RenderTarget& target,
                              RealVec2 location,
                              Color fgColor,
                              Color bgColor,
                              int frame) = 0;

  virtual std::string getViewName() = 0;

protected:
  /// Get reference to Map associated with this view.
  Map& getMap();

private:
  /// Map associated with this view.
  Map& m_map;
};