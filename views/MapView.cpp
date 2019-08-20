#include "stdafx.h"

#include "config/Settings.h"
#include "game/App.h"
#include "game/GameState.h"
#include "map/Map.h"
#include "views/MapView.h"

MapView::MapView(std::string name,
                 Map& map)
  :
  m_map(map)
{
  //startObserving(map);
}

MapView::~MapView()
{}

void MapView::setView(sf::RenderTarget & target, RealVec2 center, float zoom_level)
{
  auto& config = Config::settings();
  UintVec2 screen_size = target.getSize();
  unsigned int inventory_area_width = config.get("inventory-area-width");
  unsigned int status_area_height = config.get("status-area-height");
  unsigned int messagelog_area_height = config.get("messagelog-area-height");

  RealVec2 window_center = RealVec2((static_cast<float>(screen_size.x - inventory_area_width) / zoom_level) / 2,
                                            messagelog_area_height + (static_cast<float>(screen_size.y - (status_area_height + messagelog_area_height)) / zoom_level) / 2);

  auto rect = sf::FloatRect(static_cast<float>(center.x - window_center.x),
                            static_cast<float>(center.y - window_center.y),
                            static_cast<float>(screen_size.x / zoom_level),
                            static_cast<float>(screen_size.y / zoom_level));

  target.setView(sf::View(rect));
}

Map& MapView::getMap()
{
  return m_map;
}
