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
  /// Map of flags.
  FlagsMap flags;

  /// Map of values.
  ValuesMap values;

  /// Map of strings.
  StringsMap strings;
};


MapTileMetadata::MapTileMetadata(std::string type)
  : Metadata("maptile", type), pImpl(NEW Impl())
{
  TRACE("Loading MapTile-specific metadata for map tile \"%s\"...", type.c_str());

  pt::ptree const& data = get_ptree();

  // Look for properties section. It must be present for any MapTile.
  pt::ptree properties_tree = data.get_child("properties");
  try
  {
    properties_tree = data.get_child("properties");
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
}

MapTileMetadata::~MapTileMetadata()
{
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

