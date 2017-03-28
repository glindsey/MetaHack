#include "stdafx.h"

#include "metadata/Metadata.h"

#include "game/App.h"
#include "metadata/MetadataCollection.h"
#include "Service.h"
#include "services/IGraphicViews.h"

// Namespace aliases
namespace fs = boost::filesystem;

Metadata::Metadata(MetadataCollection& collection, std::string type)
  :
  m_collection{ collection },
  m_type{ type }
{
  std::string category = collection.get_category();

  // Look for the various files containing this metadata.
  FileName resource_string = "resources/" + category + "/" + type;
  FileName jsonfile_string = resource_string + ".json";
  FileName luafile_string = resource_string + ".lua";
  fs::path luafile_path = fs::path(luafile_string);
  fs::path jsonfile_path = fs::path(jsonfile_string);

  std::string qualified_name = category + "!" + type;

  /// Try to load this Entity's JSON metadata.
  if (fs::exists(jsonfile_path))
  {
    CLOG(INFO, "Metadata") << "Loading JSON metadata for " <<
      qualified_name;
    
    try
    {
      std::ifstream ifs(jsonfile_string);
      m_metadata << ifs;
    }
    catch (std::exception& e)
    {
      CLOG(WARNING, "Metadata") << "Error reading " << jsonfile_string << ": " << e.what();
    }
  }
  else
  {
    CLOG(WARNING, "Metadata") << "Can't find " << jsonfile_string;
  }

  /// Try to load and run this Entity's Lua script.
  if (fs::exists(luafile_path))
  {
    CLOG(INFO, "Metadata") << "Loading Lua script for " <<
      qualified_name;

    the_lua_instance.require(resource_string, true);
  }
  else
  {
    CLOG(WARNING, "Metadata") << "Can't find " << luafile_string;
  }

  Service<IGraphicViews>::get().loadViewResourcesFor(*this);
}

Metadata::~Metadata()
{}

MetadataCollection& Metadata::getMetadataCollection()
{
  return m_collection;
}

std::string const& Metadata::getType() const
{
  return m_type;
}

UintVec2 Metadata::get_tile_coords() const
{
  UintVec2 tile_location {
    getIntrinsic("tile_location_x").as<uint32_t>(),
    getIntrinsic("tile_location_y").as<uint32_t>() 
  };

  return tile_location;
}

Property Metadata::getIntrinsic(std::string name, Property default_value) const
{
  std::string group = this->getType();
  return the_lua_instance.get_group_intrinsic(group, name, default_value);
}

Property Metadata::getIntrinsic(std::string name) const
{
  return getIntrinsic(name, Property());
}

void Metadata::set_intrinsic(std::string name, Property value)
{
  std::string group = this->getType();
  the_lua_instance.set_group_intrinsic(group, name, value);
}