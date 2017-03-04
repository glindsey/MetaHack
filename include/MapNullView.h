#pragma once

#include "stdafx.h"

#include <SFML/Graphics.hpp>

#include "MapView.h"

/// Class representing a null view of a Map object.
class MapNullView : public MapView
{
  friend class NullGraphicViews;

public:
  virtual void update_tiles(EntityId viewer) override;
  virtual void update_things(EntityId viewer, int frame) override;
  
  virtual bool render(sf::RenderTexture& texture, int frame) override;

  virtual void draw_highlight(sf::RenderTarget& target,
                              Vec2f location,
                              sf::Color fgColor,
                              sf::Color bgColor,
                              int frame) override;

protected:
  /// Constructor.
  /// Private because map views should be obtained via the IGraphicViews service.
  /// @param map	Reference to Map object to associate with this view.
  MapNullView(Map& map);

  virtual void notifyOfEvent_(Observable& observed, Event event) override;

};