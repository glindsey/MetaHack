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
  return NEW EntityStandard2DView(entity, *this);
}

MapTileView* Standard2DGraphicViews::createMapTileView(MapTile& map_tile)
{
  return NEW MapTileStandard2DView(map_tile, *this);
}

MapView* Standard2DGraphicViews::createMapView(std::string name, Map& map, UintVec2 size)
{
  return NEW MapStandard2DView(name, map, size, *this);
}

TileSheet& Standard2DGraphicViews::getTileSheet()
{
  return *(m_tileSheet.get());
}

bool Standard2DGraphicViews::needToLoadFilesFor(std::string category)
{
  return (m_noTilesAvailable.count(category) != 0) && (m_tileCoords.count(category) != 0);
}

UintVec2 const& Standard2DGraphicViews::getTileSheetCoords(std::string category)
{
  if (!needToLoadFilesFor(category))
  {
    loadViewResourcesFor(category);
  }

  return m_tileCoords.at(category);
}

void Standard2DGraphicViews::loadViewResourcesFor(std::string category)
{
  FileName resourceString = "resources/entity/" + category;
  FileName pngFileString = resourceString + ".png";
  fs::path pngFilePath = fs::path(pngFileString);

  if (fs::exists(pngFilePath))
  {
    UintVec2 tileLocation;
    CLOG(TRACE, "TileSheet") << "Tiles were found for " << category << " category";

    tileLocation = m_tileSheet->loadCollection(pngFileString);
    CLOG(TRACE, "TileSheet") << "Tiles for " << category <<
      " were placed on the TileSheet at " << tileLocation;

    m_tileCoords[category] = tileLocation;
    m_noTilesAvailable.erase(category);
  }
  else
  {
    CLOG(TRACE, "TileSheet") << "No tiles found for " << category;
    m_noTilesAvailable.insert(category);
  }
}