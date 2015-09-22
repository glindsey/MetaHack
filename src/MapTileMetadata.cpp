#include "MapTileMetadata.h"

#include <unordered_map>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "ErrorHandler.h"

#define BOOST_FILESYSTEM_NO_DEPRECATED

// Static declarations
std::map< std::string, std::unique_ptr<MapTileMetadata> > MapTileMetadata::collection;

// Namespace aliases
namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

// Using declarations
using FlagsMap = std::map < std::string, bool >;
using ValuesMap = std::map < std::string, int >;
using StringsMap = std::map < std::string, std::string >;

struct MapTileMetadata::Impl
{
  /// Thing's own name.
  std::string name;

  /// Thing pretty name.
  std::string pretty_name;

  /// Thing description
  std::string description;

  /// Map of flags.
  FlagsMap flags;

  /// Map of values.
  ValuesMap values;

  /// Map of strings.
  StringsMap strings;

};


MapTileMetadata::MapTileMetadata(std::string type)
  : pImpl(new Impl())
{
  TRACE("Loading metadata for map tile \"%s\"...", type.c_str());

  pImpl->name = type;

  /// Try to open the XML file to populate this thing's metadata.
  std::string maptile_string = "resources/maptiles/" + type + ".xml";
  fs::path maptile_path = fs::path(maptile_string);

  if (fs::exists(maptile_path))
  {
    //TRACE("Found file \"%s\"", thing_string.c_str());

    /// Load file.
    pt::ptree data;
    pt::xml_parser::read_xml(maptile_string, data);

    //TRACE("Loaded property tree for \"%s\"", type.c_str());

    // Get thing's pretty name.
    try
    {
      pImpl->pretty_name = data.get_child("maptile.name").get_value<std::string>("[" + type + "]");
    }
    catch (pt::ptree_bad_path&)
    {
      pImpl->pretty_name = "[" + type + "]";
    }

    // Get thing's description, if present. Otherwise set it equal to the name.
    try
    {
      pImpl->description = data.get_child("maptile.description").get_value<std::string>(pImpl->pretty_name);
    }
    catch (pt::ptree_bad_path&)
    {
      pImpl->description = pImpl->pretty_name;
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
  }
  else
  {
    FATAL_ERROR("File \"%s\" not found", maptile_string.c_str());
  }
}

MapTileMetadata& MapTileMetadata::get(std::string type)
{
  try
  {
    return *(collection.at(type));
  }
  catch (std::out_of_range&)
  {
    collection.emplace(std::make_pair(type, std::unique_ptr<MapTileMetadata>(new MapTileMetadata(type))));
  }
}


MapTileMetadata::~MapTileMetadata()
{
}

std::string const& MapTileMetadata::get_pretty_name() const
{
  return pImpl->pretty_name;
}

std::string const& MapTileMetadata::get_description() const
{
  return pImpl->description;
}

bool MapTileMetadata::get_flag(std::string key, bool default_value) const
{
  boost::algorithm::to_lower(key);

  try
  {
    return pImpl->flags.at(key);
  }
  catch (std::out_of_range&)
  {
    return default_value;
  }
}

int MapTileMetadata::get_value(std::string key, int default_value) const
{
  boost::algorithm::to_lower(key);

  try
  {
    return pImpl->values.at(key);
  }
  catch (std::out_of_range&)
  {
    return default_value;
  }
}

std::string MapTileMetadata::get_string(std::string key, std::string default_value) const
{
  boost::algorithm::to_lower(key);

  try
  {
    return pImpl->strings.at(key);
  }
  catch (std::out_of_range&)
  {
    return default_value;
  }
}