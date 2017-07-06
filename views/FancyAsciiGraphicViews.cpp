#include "stdafx.h"

#include "views/FancyAsciiGraphicViews.h"

#include "AssertHelper.h"
#include "services/Service.h"
#include "services/IConfigSettings.h"
#include "services/IGameRules.h"
#include "tilesheet/TileSheet.h"
#include "utilities/New.h"
#include "utilities/StringTransforms.h"
#include "views/EntityFancyAsciiView.h"
#include "views/MapTileFancyAsciiView.h"
#include "views/MapFancyAsciiView.h"

// Namespace aliases
namespace fs = boost::filesystem;

FancyAsciiGraphicViews::FancyAsciiGraphicViews()
{
  auto& config = S<IConfigSettings>();
  auto tileSize = config.get("ascii-tile-size");
  auto textureSize = config.get("tilesheet-texture-size");
  auto textureFilename = config.get("ascii-tiles-filename").get<std::string>();

  m_tileSheet.reset(NEW TileSheet(tileSize, textureSize));
  m_tileSheet->loadCollection("resources/graphic/" + textureFilename);
}

FancyAsciiGraphicViews::~FancyAsciiGraphicViews()
{
}

EntityView* FancyAsciiGraphicViews::createEntityView(EntityId entity)
{
  return NEW EntityFancyAsciiView(entity, *this);
}

MapTileView* FancyAsciiGraphicViews::createMapTileView(MapTile& map_tile)
{
  return NEW MapTileFancyAsciiView(map_tile, *this);
}

MapView* FancyAsciiGraphicViews::createMapView(metagui::Desktop& desktop,
                                               std::string name, 
                                               Map& map, 
                                               UintVec2 size)
{
  return NEW MapFancyAsciiView(desktop, name, map, size, *this);
}

void FancyAsciiGraphicViews::loadViewResourcesFor(std::string category)
{
  m_triedToLoad.insert(category);

  auto& categoryData = S<IGameRules>().categoryData(category);

  auto& componentData = categoryData["components"];
  if (componentData.count("appearance") == 0) return;

  auto& appearanceData = componentData.at("appearance");
  if (appearanceData.count("ascii") == 0) return;

  unsigned int asciiValue = appearanceData["ascii"];
  m_tileCoords[category] = UintVec2(asciiValue % 16, asciiValue / 16);
}

TileSheet& FancyAsciiGraphicViews::getTileSheet()
{
  return *(m_tileSheet.get());
}

/// @todo This can be improved by caching the required data.
bool FancyAsciiGraphicViews::hasTilesFor(std::string category)
{
  if (m_triedToLoad.count(category) == 0)
  {
    loadViewResourcesFor(category);
  }

  return m_tileCoords.count(category) != 0;
}

UintVec2 const& FancyAsciiGraphicViews::getTileSheetCoords(std::string category)
{
  bool hasTiles = hasTilesFor(category);

  Assert("TileSheet", hasTiles != 0,
         "getTileSheetCoords(\"" << category << "\") called, but requested category has no tiles");

  return m_tileCoords.at(category);
}
