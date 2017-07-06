#pragma once

#include <SFML/Graphics.hpp>

#include "views/MapTileView.h"

// Forward declarations

/// Class representing a null view of a MapTile object.
class MapTileNullView : public MapTileView
{
  friend class NullGraphicViews;

public:
  virtual std::string getViewName() override;

protected:
  /// Constructor.
  /// @param map	Reference to Map object to associate with this view.
  MapTileNullView(MapTile& map_tile);

  virtual bool onEvent(Event const& event) override;
};