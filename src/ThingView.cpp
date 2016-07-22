#include "stdafx.h"

#include "ThingView.h"

#include "App.h"
#include "ConfigSettings.h"
#include "GameState.h"
#include "ThingManager.h"

ThingView::ThingView()
	:
	m_thing_id(ThingManager::get_mu())
{}

ThingView::ThingView(ThingId thing_id)
	:
	m_thing_id(thing_id)
{}

ThingView::~ThingView()
{}

void ThingView::set_thing_id(ThingId thing_id)
{
	if (thing_id != m_thing_id)
	{
		m_thing_id = thing_id;
		reset_cached_render_data();
	}
}

ThingId ThingView::get_thing_id()
{
	return m_thing_id;
}

Thing& ThingView::get_thing()
{
	return GAME.get_things().get(m_thing_id);
}