#pragma once

#include "stdafx.h"

#include <SFML/Graphics.hpp>

#include "MapTileView.h"

// Forward declarations

/// Class representing a null view of a MapTile object.
class MapTileNullView : public MapTileView
{
  friend class NullGraphicViews;

public:
  //virtual bool render(sf::RenderTexture& texture, int frame) override;

  virtual std::string get_view_name() override;

protected:
  /// Constructor.
  /// @param map	Reference to Map object to associate with this view.
  MapTileNullView(MapTile& map_tile);

};