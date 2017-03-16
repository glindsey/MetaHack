#include "stdafx.h"

#include "MapNullView.h"
#include "MapTileNullView.h"
#include "New.h"

MapNullView::MapNullView(std::string name, Map& map, Vec2u size)
  :
  MapView(name, map, size)
{
}

void MapNullView::update_tiles(EntityId viewer)
{
}

void MapNullView::update_things(EntityId viewer, int frame)
{
}

bool MapNullView::render_map(sf::RenderTexture& texture, int frame)
{
  return true;
}

void MapNullView::draw_highlight(sf::RenderTarget& target,
                                 RealVec2 location,
                                 sf::Color fgColor,
                                 sf::Color bgColor,
                                 int frame)
{
}

std::string MapNullView::getViewName()
{
  return "null";
}