#pragma once

#include "stdafx.h"

#include "MapTile.h"
#include "Observer.h"

// Forward declarations
class ThingId;

/// Abstract class representing a view of a MapTile object.
template< class ThingViewSubclass >
class MapTileView
  :
  public Observer
{
public:
  /// Constructor.
  /// @param map	Reference to MapTile object to associate with this view.
  explicit MapTileView(MapTile& map_tile)
    :
    m_map_tile(map_tile)
  {
    startObserving(map_tile);
  }

  /// Destructor.
  virtual ~MapTileView() {}

protected:
  /// Get reference to MapTile associated with this view.
  MapTile& get_map_tile()
  {
    return m_map_tile;
  }

  /// Add thing view to vector.
  /// @note The MapTileView instance will *assume ownership* of the view.
  void add_thing_view(ThingViewSubclass* thing_view)
  {
    auto& iter = std::find(m_thing_views.begin(), m_thing_views.end(), thing_view);
    if (iter == m_thing_views.end())
    {
      m_thing_views.push_back(thing_view);
    }
  }

  /// Delete a thing view from vector.
  /// @param thing_view Pointer to view to delete.
  /// @return True if the view was found and deleted, false if not.
  /// @warning The pointer passed in will *no longer be valid* if the view is found and deleted!
  bool delete_thing_view(ThingViewSubclass* thing_view)
  {
    auto& iter = std::find(m_thing_views.begin(), m_thing_views.end(), thing_view);
    if (iter != m_thing_views.end())
    {     
      m_thing_views.erase(iter);
    }
  }

  /// Clear thing views vector.
  void clear_thing_views()
  {
    m_thing_views.clear();
  }

  /// Get vector of thing views.
  std::vector<ThingViewSubclass> const& get_thing_views()
  {
    return m_thing_views;
  }

  MapTile const& get_map_tile() const
  {
    return m_map_tile;
  }

  virtual void notifyOfEvent_(Observable& observed, Event event) = 0;

private:
  /// MapTile associated with this view.
  MapTile& m_map_tile;

  /// Vector of Thing views.
  std::vector<ThingViewSubclass> m_thing_views;
};