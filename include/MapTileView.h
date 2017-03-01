#pragma once

#include "stdafx.h"

#include "MapTile.h"
#include "Observer.h"

// Forward declarations
class ThingId;

/// Abstract class representing a view of a MapTile object.
class MapTileView 
  :
  public Observer
{
public:
  /// Constructor.
  /// @param map	Reference to MapTile object to associate with this view.
  explicit MapTileView(MapTile& map_tile);

  /// Destructor.
  virtual ~MapTileView();

protected:
  /// Get reference to MapTile associated with this view.
  MapTile& get_map_tile();

  MapTile const& get_map_tile() const;

  virtual void notifyOfEvent_(Observable& observed, Event event) = 0;

private:
  /// MapTile associated with this view.
  MapTile& m_map_tile;
};