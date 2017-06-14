#include "stdafx.h"

#include "services/Standard2DGraphicViews.h"

#include "entity/EntityStandard2DView.h"
#include "maptile/MapTileStandard2DView.h"
#include "map/MapStandard2DView.h"
#include "services/Service.h"
#include "services/IConfigSettings.h"
#include "tilesheet/TileSheet.h"
#include "utilities/New.h"

// Namespace aliases
namespace fs = boost::filesystem;

Standard2DGraphicViews::Standard2DGraphicViews()
{
  auto& config = S<IConfigSettings>();
  auto tileSize = config.get("map-tile-size");
  auto textureSize = config.get("tilesheet-texture-size");

  m_tileSheet.reset(NEW TileSheet(tileSize, textureSize));
}

Standard2DGraphicViews::~Standard2DGraphicViews()
{
}

EntityView* Standard2DGraphicViews::createEntityView(EntityId entity)
{
  return NEW EntityStandard2DView(entity, getTileSheet());
}

MapTileView* Standard2DGraphicViews::createMapTileView(MapTile& map_tile)
{
  return NEW MapTileStandard2DView(map_tile, getTileSheet());
}

MapView* Standard2DGraphicViews::createMapView(std::string name, Map& map, UintVec2 size)
{
  return NEW MapStandard2DView(name, map, size, getTileSheet());
}

TileSheet& Standard2DGraphicViews::getTileSheet()
{
  return *(m_tileSheet.get());
}

void Standard2DGraphicViews::loadViewResourcesFor(std::string category, json& data)
{
  FileName resource_string = "resources/entity/" + category;
  FileName pngfile_string = resource_string + ".png";
  fs::path pngfile_path = fs::path(pngfile_string);

  if (fs::exists(pngfile_path))
  {
    UintVec2 tile_location;
    CLOG(TRACE, "GameState") << "Tiles were found for " << category << " category";

    tile_location = m_tileSheet->load_collection(pngfile_string);
    CLOG(TRACE, "GameState") << "Tiles for " << category <<
      " were placed on the TileSheet at " << tile_location;

    data["has-tiles"] = true;
    data["tile-location"] = tile_location;
  }
  else
  {
    CLOG(TRACE, "GameState") << "No tiles found for " << category;
  }
}