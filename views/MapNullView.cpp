#include "stdafx.h"

#include "views/MapNullView.h"

#include "utilities/New.h"
#include "views/MapTileNullView.h"

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

bool MapNullView::renderMap(sf::RenderTexture& texture, int frame)
{
  return true;
}

void MapNullView::drawHighlight(sf::RenderTarget& target,
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