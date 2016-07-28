#include "stdafx.h"

#include "MapView.h"

#include "App.h"
#include "ConfigSettings.h"
#include "GameState.h"

MapView::MapView()
  :
  m_map_id(MapFactory::null_map_id)
{}

MapView::MapView(MapId map_id)
  :
  m_map_id(map_id)
{}

MapView::~MapView()
{}

void MapView::set_map_id(MapId map_id)
{
  if (map_id != m_map_id)
  {
    m_map_id = map_id;
    reset_cached_render_data();
  }
}

MapId MapView::get_map_id()
{
  return m_map_id;
}

void MapView::set_view(sf::RenderTarget & target, sf::Vector2f center, float zoom_level)
{
  sf::Vector2u screen_size = target.getSize();
  unsigned int inventory_area_width = the_config.get<unsigned int>("inventory_area_width");
  unsigned int status_area_height = the_config.get<unsigned int>("status_area_height");
  unsigned int messagelog_area_height = the_config.get<unsigned int>("messagelog_area_height");

  sf::Vector2f window_center = sf::Vector2f((static_cast<float>(screen_size.x - inventory_area_width) / zoom_level) / 2,
                                            messagelog_area_height + (static_cast<float>(screen_size.y - (status_area_height + messagelog_area_height)) / zoom_level) / 2);

  target.setView(sf::View(sf::FloatRect((center.x - window_center.x),
    (center.y - window_center.y),
                                        (screen_size.x / zoom_level),
                                        (screen_size.y / zoom_level))));
}

Map& MapView::get_map()
{
  return GAME.get_maps().get(m_map_id);
}