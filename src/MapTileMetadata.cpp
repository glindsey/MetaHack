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
boost::ptr_unordered_map<std::string, MapTileMetadata> MapTileMetadata::collection;

// Namespace aliases
namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

struct MapTileMetadata::Impl
{
  /// Thing's own name.
  std::string name;

  /// Thing pretty name.
  std::string display_name;

  /// Thing description
  std::string description;

  /// Map of flags.
  FlagsMap flags;

  /// Map of values.
  ValuesMap values;

  /// Map of strings.
  StringsMap strings;

  /// Boolean indicating whether this MapTile has graphics associated with it.
  bool has_tiles;

  /// Location of this MapTile's graphics on the tilesheet.
  sf::Vector2u tile_location;
};


MapTileMetadata::MapTileMetadata(std::string type)
  : pImpl(NEW Impl())
{
  TRACE("Loading metadata for map tile \"%s\"...", type.c_str());

  pImpl->name = type;

  // Look for the XML/PNG files for this maptile.
  std::string xmlfile_string = "resources/maptiles/" + type + ".xml";
  fs::path xmlfile_path = fs::path(xmlfile_string);
  std::string pngfile_string = "resources/maptiles/" + type + ".png";
  fs::path pngfile_path = fs::path(pngfile_string);

  if (!fs::exists(xmlfile_path))
  {
    throw ExceptionMissingFile("XML", "MapTile", type);
  }

  //TRACE("Found file \"%s\"", thing_string.c_str());

  /// Load file.
  pt::ptree data;
  pt::xml_parser::read_xml(xmlfile_string, data);

  //TRACE("Loaded property tree for \"%s\"", type.c_str());

  // Get thing's pretty name.
  try
  {
    pImpl->display_name = data.get_child("maptile.name").get_value<std::string>("[" + type + "]");
  }
  catch (pt::ptree_bad_path&)
  {
    pImpl->display_name = "[" + type + "]";
  }

  // Get thing's description, if present. Otherwise set it equal to the name.
  try
  {
    pImpl->description = data.get_child("maptile.description").get_value<std::string>(pImpl->display_name);
  }
  catch (pt::ptree_bad_path&)
  {
    pImpl->description = pImpl->display_name;
  }

  // Look for properties section. It must be present for any MapTile.
  pt::ptree properties_tree = data.get_child("maptile.properties");
  try
  {
    properties_tree = data.get_child("maptile.properties");
  }
  catch (pt::ptree_bad_path& p)
  {
    FATAL_ERROR(p.what());
  }

  // Get flags.
  try
  {
    for (auto& child_tree : properties_tree.get_child("flags"))
    {
      std::string key = child_tree.first;
      boost::algorithm::to_lower(key);
      bool value = child_tree.second.get_value<bool>(false);

      pImpl->flags[key] = value;
    }
  }
  catch (pt::ptree_bad_path&)
  {
  }

  // Get values.
  try
  {
    for (auto& child_tree : properties_tree.get_child("values"))
    {
      std::string key = child_tree.first;
      boost::algorithm::to_lower(key);
      int value = child_tree.second.get_value<int>(0);

      pImpl->values[key] = value;
    }
  }
  catch (pt::ptree_bad_path&)
  {
  }

  // Get strings.
  try
  {
    pt::ptree intrinsic_strings = properties_tree.get_child("strings");

    for (auto& child_tree : properties_tree.get_child("strings"))
    {
      std::string key = child_tree.first;
      boost::algorithm::to_lower(key);
      std::string value = child_tree.second.get_value<std::string>("");

      pImpl->strings[key] = value;
    }
  }
  catch (pt::ptree_bad_path&)
  {
  }

  if (fs::exists(pngfile_path))
  {
    pImpl->has_tiles = true;
    pImpl->tile_location = TS.load_collection(pngfile_string);
    TRACE("Tiles for MapTile %s were placed on the TileSheet at (%u, %u)",
      type.c_str(), pImpl->tile_location.x, pImpl->tile_location.y);
  }
  else
  {
    pImpl->has_tiles = false;
  }
}

MapTileMetadata* MapTileMetadata::get(std::string type)
{
  if (type.empty())
  {
    type = "Unknown";
  }

  if (collection.count(type) == 0)
  {
    collection.insert(type, NEW MapTileMetadata(type));
  }

  return &(collection.at(type));
}


MapTileMetadata::~MapTileMetadata()
{
}

std::string const& MapTileMetadata::get_display_name() const
{
  return pImpl->display_name;
}

std::string const& MapTileMetadata::get_description() const
{
  return pImpl->description;
}

bool MapTileMetadata::get_flag(std::string key, bool default_value) const
{
  boost::algorithm::to_lower(key);

  if (pImpl->flags.count(key) != 0)
  {
    return pImpl->flags.at(key);
  }
  else
  {
    return default_value;
  }
}

int MapTileMetadata::get_value(std::string key, int default_value) const
{
  boost::algorithm::to_lower(key);

  if (pImpl->values.count(key) != 0)
  {
    return pImpl->values.at(key);
  }
  else
  {
    return default_value;
  }
}

std::string MapTileMetadata::get_string(std::string key, std::string default_value) const
{
  boost::algorithm::to_lower(key);

  if (pImpl->strings.count(key) != 0)
  {
    return pImpl->strings.at(key);
  }
  else
  {
    return default_value;
  }
}

sf::Vector2u MapTileMetadata::get_tile_coords() const
{
  return pImpl->tile_location;
}