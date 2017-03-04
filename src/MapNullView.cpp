#include "stdafx.h"

#include "MapNullView.h"
#include "MapTileNullView.h"
#include "New.h"

MapNullView::MapNullView(Map& map)
  :
  MapView(map)
{
}

void MapNullView::update_tiles(EntityId viewer)
{
}

void MapNullView::update_things(EntityId viewer, int frame)
{
}

bool MapNullView::render(sf::RenderTexture& texture, int frame)
{
  return true;
}

void MapNullView::draw_highlight(sf::RenderTarget& target,
                                 Vec2f location,
                                 sf::Color fgColor,
                                 sf::Color bgColor,
                                 int frame)
{
}

void MapNullView::notifyOfEvent_(Observable & observed, Event event)
{
  /// @todo WRITE ME
}
