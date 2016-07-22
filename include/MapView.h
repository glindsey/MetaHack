#pragma once

#include "stdafx.h"

#include "Map.h"
#include "Renderable.h"


// Forward declarations
class ThingId;

/// Abstract cLass representing a view of a Map object.
class MapView : public RenderableToTexture
{
public:
	/// Constructor.
	/// @param map	ID of Map object to associate with this view.
	MapView(MapId map_id);

	/// Update any cached render data associated with map tiles.
	/// @param thing	ID of the thing that is percieving the map.
	virtual void update_tiles(ThingId thing) = 0;

	/// Update any cached render data associated with map things.
	/// @param thing	ID of the thing that is percieving the map.
	/// @param frame	Current animation frame number.
	virtual void update_things(ThingId thing, int frame) = 0;

protected:
	/// Get reference to Map associated with this view.
	Map& get_map();

private:
	/// Map ID associated with this view.
	MapId m_map_id;
};