#include "stdafx.h"

#include "Metadata.h"

#include "ErrorHandler.h"
#include "MetadataCollection.h"

// Namespace aliases
namespace fs = boost::filesystem;

Metadata::Metadata(MetadataCollection& collection, std::string type)
  :
  m_collection{ collection },
  m_type{ type }
{
  SET_UP_LOGGER("Metadata", false);

  std::string category = collection.get_category();

  // Look for the various files containing this metadata.
  FileName resource_string = "resources/" + category + "s/" + type;
  FileName pngfile_string = resource_string + ".png";
  fs::path pngfile_path = fs::path(pngfile_string);
  FileName luafile_string = resource_string + ".lua";
  fs::path luafile_path = fs::path(luafile_string);

  std::string qualified_name = category + "!" + type;

  /// Try to load and run this Entity's Lua script.
  if (fs::exists(luafile_path))
  {
    CLOG(INFO, "Metadata") << "Loading Lua script for " <<
      qualified_name;

    the_lua_instance.require(resource_string, true);
  }
  else
  {
    LOG(FATAL) << "Can't find " << luafile_string;
  }

  if (fs::exists(pngfile_path))
  {
    Vec2u tile_location;
    CLOG(TRACE, "Metadata") << "Tiles were found for " << qualified_name;

    tile_location = the_tilesheet.load_collection(pngfile_string);
    CLOG(TRACE, "Metadata") << "Tiles for " << qualified_name <<
      " were placed on the TileSheet at " << tile_location;

    set_intrinsic<bool>("has_tiles", true);
    set_intrinsic<int>("tile_location_x", tile_location.x);
    set_intrinsic<int>("tile_location_y", tile_location.y);
  }
  else
  {
    CLOG(TRACE, "Metadata") << "No tiles found for " << qualified_name;
  }
}

Metadata::~Metadata()
{}

MetadataCollection& Metadata::get_metadata_collection()
{
  return m_collection;
}

std::string const& Metadata::get_type() const
{
  return m_type;
}

Vec2u Metadata::get_tile_coords() const
{
  Vec2u tile_location;
  tile_location.x = get_intrinsic<int>("tile_location_x");
  tile_location.y = get_intrinsic<int>("tile_location_y");
  return tile_location;
}