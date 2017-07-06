#pragma once

#include <SFML/Graphics.hpp>

#include "views/MapView.h"

/// Class representing a null view of a Map object.
class MapNullView : public MapView
{
  friend class NullGraphicViews;

public:
  virtual void updateTiles(EntityId viewer, Systems::Lighting& lighting) override;
  virtual void updateEntities(EntityId viewer, Systems::Lighting& lighting, int frame) override;

  virtual bool render_map(sf::RenderTexture& texture, int frame) override;

  virtual void draw_highlight(sf::RenderTarget& target,
                              RealVec2 location,
                              Color fgColor,
                              Color bgColor,
                              int frame) override;

  virtual std::string getViewName() override;

protected:
  /// Constructor.
  /// Private because map views should be obtained via the IGraphicViews service.
  /// @param name   The name of this view.
  /// @param map	Reference to Map object to associate with this view.
  MapNullView(metagui::Desktop& desktop, 
              std::string name, 
              Map& map, 
              UintVec2 size);

};
