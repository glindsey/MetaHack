#pragma once

#include "stdafx.h"

#include "maptile/MapTile.h"

// Forward declarations
class EntityId;

/// Abstract class representing a view of a MapTile object.
/// Does NOT represent the Thing objects that might be on the tile.
class MapTileView
  :
  public Object
{
  friend class NullGraphicViews;
  friend class Standard2DGraphicViews;

public:
  /// Destructor.
  virtual ~MapTileView() {}

  virtual std::string getViewName() = 0;

protected:
  /// Constructor.
  /// @param map	Reference to MapTile object to associate with this view.
  explicit MapTileView(MapTile& map_tile)
    :
    Object({}),
    m_map_tile(map_tile)
  {
    //startObserving(map_tile);
  }

  /// Get reference to MapTile associated with this view.
  MapTile& get_map_tile()
  {
    return m_map_tile;
  }

  MapTile const& get_map_tile() const
  {
    return m_map_tile;
  }

private:
  /// MapTile associated with this view.
  MapTile& m_map_tile;
};