#pragma once

#include "stdafx.h"

#include <SFML/Graphics.hpp>

#include "MapView.h"


// Forward declarations

/// Class representing the standard 2D (pseudo-3D) view of a Map object.
class MapStandard2DView : public MapView
{
public:
	/// Constructor.
	/// @param map	ID of Map object to associate with this view.
	MapStandard2DView(MapId map_id);

	virtual void update_tiles(ThingId thing) override;
	virtual void update_things(ThingId thing, int frame) override;
	virtual bool render(sf::RenderTexture& texture, int frame) override;

protected:

private:
	/// "Seen" map vertex array.
	sf::VertexArray m_map_seen_vertices;

	/// Outlines map vertex array.
	sf::VertexArray m_map_outline_vertices;

	/// "Memory" map vertex array.
	sf::VertexArray m_map_memory_vertices;

	/// Thing vertex array.
	sf::VertexArray m_thing_vertices;

};