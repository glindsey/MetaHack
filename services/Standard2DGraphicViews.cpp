#include "stdafx.h"

#include "services/Standard2DGraphicViews.h"

#include "AssertHelper.h"
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

bool Standard2DGraphicViews::hasTilesFor(std::string category)
{
  if (needToLoadFilesFor(category))
  {
    loadViewResourcesFor(category);
  }

  return (m_tileCoords.count(category) != 0);
}

bool Standard2DGraphicViews::needToLoadFilesFor(std::string category)
{
  return (m_triedToLoad.count(category) == 0);
}

UintVec2 const& Standard2DGraphicViews::getTileSheetCoords(std::string category)
{
  if (needToLoadFilesFor(category))
  {
    loadViewResourcesFor(category);
  }

  Assert("TileSheet", m_tileCoords.count(category) != 0,
         "getTileSheetCoords(\"" << category << "\") called, but requested category has no tiles");

  return m_tileCoords.at(category);
}

void Standard2DGraphicViews::loadViewResourcesFor(std::string category)
{
  FileName resourceString = "resources/entity/" + category;
  FileName pngFileString = resourceString + ".png";
  fs::path pngFilePath = fs::path(pngFileString);
  m_triedToLoad.insert(category);

  if (fs::exists(pngFilePath))
  {
    UintVec2 tileLocation;
    CLOG(TRACE, "TileSheet") << "Tiles were found for " << category << " category";

    tileLocation = m_tileSheet->loadCollection(pngFileString);
    CLOG(TRACE, "TileSheet") << "Tiles for " << category <<
      " were placed on the TileSheet at " << tileLocation;

    m_tileCoords[category] = tileLocation;
  }
  else
  {
    CLOG(TRACE, "TileSheet") << "No tiles found for " << category;
    /// @note No way to actually "erase" a tile from a tilesheet right now, so
    ///       this would end up leaving an unusable "gap" in the sheet. But
    ///       the only way this code should be called is if the PNG file were
    ///       erased *during* program run, and the views reset.
    m_tileCoords.erase(category);
  }
}