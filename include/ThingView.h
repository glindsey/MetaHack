#pragma once

#include "stdafx.h"

#include "Thing.h"
#include "Renderable.h"


/// Abstract cLass representing a view of a Thing object.
class ThingView : public RenderableToTexture
{
public:
	/// Constructor.
	ThingView();

	/// Constructor.
	/// @param thing	ID of Thing object to associate with this view.
	explicit ThingView(ThingId thing_id);

	/// Destructor.
	virtual ~ThingView();

	/// Sets the current thing ID.
	/// If new thing ID is different from the current thing ID,
	/// this method may reinitialize cached render data.
	/// @param thing_id ID of the thing to view.
	void set_thing_id(ThingId thing_id);

	/// Gets the current thing ID.
	/// @return The ID of the thing being viewed.
	ThingId get_thing_id();

	/// @todo Method to set location to draw to on texture.

	/// Update any cached render data associated with thing.
	/// @param thing	ID of the thing that is percieving the map.
	virtual void update_render_data(ThingId thing) = 0;

protected:
	/// Get reference to Thing associated with this view.
	Thing& get_thing();

	/// Reinitialize cached map render data.
	virtual void reset_cached_render_data() = 0;

private:
	/// Thing ID associated with this view.
	ThingId m_thing_id;
};