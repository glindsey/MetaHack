#include "stdafx.h"

#include "map/MapNullView.h"
#include "maptile/MapTileNullView.h"
#include "utilities/New.h"

MapNullView::MapNullView(std::string name, Map& map, UintVec2 size)
  :
  MapView(name, map, size)
{
}

bool MapNullView::onEvent_NVI_PreChildren(Event const& event)
{
  return true;
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
                                 Color fgColor,
                                 Color bgColor,
                                 int frame)
{
}

std::string MapNullView::getViewName()
{
  return "null";
}