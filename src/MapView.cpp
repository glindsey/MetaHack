#include "stdafx.h"

#include "MapView.h"

#include "App.h"
#include "GameState.h"
#include "IConfigSettings.h"
#include "Service.h"

MapView::MapView(Map& map)
  :
  m_map(map)
{
  //startObserving(map);
}

MapView::~MapView()
{}

void MapView::set_view(sf::RenderTarget & target, Vec2f center, float zoom_level)
{
  auto& config = Service<IConfigSettings>::get();
  Vec2u screen_size = target.getSize();
  unsigned int inventory_area_width = config.get<unsigned int>("inventory_area_width");
  unsigned int status_area_height = config.get<unsigned int>("status_area_height");
  unsigned int messagelog_area_height = config.get<unsigned int>("messagelog_area_height");

  Vec2f window_center = Vec2f((static_cast<float>(screen_size.x - inventory_area_width) / zoom_level) / 2,
                                            messagelog_area_height + (static_cast<float>(screen_size.y - (status_area_height + messagelog_area_height)) / zoom_level) / 2);

  target.setView(sf::View(sf::FloatRect((center.x - window_center.x),
    (center.y - window_center.y),
                                        (screen_size.x / zoom_level),
                                        (screen_size.y / zoom_level))));
}

Map& MapView::get_map()
{
  return m_map;
}