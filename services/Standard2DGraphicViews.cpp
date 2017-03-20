#include "stdafx.h"

#include "services/Standard2DGraphicViews.h"

#include "entity/EntityStandard2DView.h"
#include "maptile/MapTileStandard2DView.h"
#include "map/MapStandard2DView.h"
#include "Service.h"
#include "services/IConfigSettings.h"
#include "tilesheet/TileSheet.h"
#include "utilities/New.h"

// Namespace aliases
namespace fs = boost::filesystem;

Standard2DGraphicViews::Standard2DGraphicViews()
{
  auto& config = Service<IConfigSettings>::get();
  auto tileSize = config.get("map_tile_size").as<int32_t>();
  auto textureSize = config.get("tilesheet_texture_size").as<int32_t>();

  m_tile_sheet.reset(NEW TileSheet(tileSize, textureSize));
}

Standard2DGraphicViews::~Standard2DGraphicViews()
{
}

EntityView* Standard2DGraphicViews::createEntityView(Entity& entity)
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
  return *(m_tile_sheet.get());
}

void Standard2DGraphicViews::loadViewResourcesFor(Metadata& metadata)
{
  auto category = metadata.get_metadata_collection().get_category();
  auto type = metadata.get_type();
  FileName resource_string = "resources/" + category + "/" + type;
  FileName pngfile_string = resource_string + ".png";
  fs::path pngfile_path = fs::path(pngfile_string);
  std::string qualified_name = category + "!" + type;

  if (fs::exists(pngfile_path))
  {
    UintVec2 tile_location;
    CLOG(TRACE, "Metadata") << "Tiles were found for " << qualified_name;

    tile_location = m_tile_sheet->load_collection(pngfile_string);
    CLOG(TRACE, "Metadata") << "Tiles for " << qualified_name <<
      " were placed on the TileSheet at " << tile_location;

    metadata.set_intrinsic("has_tiles", Property::from(true));
    metadata.set_intrinsic("tile_location_x", Property::from(tile_location.x));
    metadata.set_intrinsic("tile_location_y", Property::from(tile_location.y));
  }
  else
  {
    CLOG(TRACE, "Metadata") << "No tiles found for " << qualified_name;
  }
}
