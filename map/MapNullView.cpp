#include "stdafx.h"

#include "map/MapNullView.h"
#include "maptile/MapTileNullView.h"
#include "utilities/New.h"

MapNullView::MapNullView(metagui::Desktop& desktop, std::string name, Map& map, UintVec2 size)
  :
  MapView(desktop, name, map, size)
{
}

void MapNullView::updateTiles(EntityId viewer, Systems::Lighting& lighting)
{
}

void MapNullView::updateEntities(EntityId viewer, Systems::Lighting& lighting, int frame)
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