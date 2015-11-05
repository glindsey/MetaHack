#include "MapTileMetadata.h"

#include <unordered_map>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <SFML/Graphics.hpp>

#include "common_types.h"

#include "App.h"
#include "ErrorHandler.h"
#include "Exceptions.h"
#include "TileSheet.h"

#define BOOST_FILESYSTEM_NO_DEPRECATED

// Static declarations
boost::ptr_unordered_map<std::string, MapTileMetadata> MapTileMetadata::s_collection;

// Namespace aliases
namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

MapTileMetadata::MapTileMetadata(std::string type)
  : Metadata("maptile", type)
{}

MapTileMetadata::~MapTileMetadata()
{}

MapTileMetadata* MapTileMetadata::get(std::string type)
{
  if (type.empty())
  {
    type = "Unknown";
  }

  if (s_collection.count(type) == 0)
  {
    s_collection.insert(type, NEW MapTileMetadata(type));
  }

  return &(s_collection.at(type));
}