#include "stdafx.h"

#include "metadata/Metadata.h"

#include "game/App.h"
#include "metadata/MetadataCollection.h"
#include "Service.h"
#include "services/IGraphicViews.h"
#include "utilities/RNGUtils.h"
#include "utilities/StringTransforms.h"

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
      CLOG(FATAL, "Metadata") << "Error reading " <<
        jsonfile_string << ": " << e.what();
    }
  }
  else
  {
    CLOG(WARNING, "Metadata") << "Can't find " << jsonfile_string;
  }

  /// Check the parent key.
  if (m_metadata.count("parent") != 0)
  {
    std::string parentName = m_metadata["parent"].get<std::string>();
    if (parentName == type)
    {
      CLOG(FATAL, "Metadata") << qualified_name << " is defined as its own parent. What do you think this is, a Heinlein story?";
    }
    Metadata& parentData = collection.get(parentName);

    add(parentData);
  }

  // DEBUG: Check for "Human".
  if (type == "Human")
  {
    CLOG(TRACE, "Metadata") << "================================";
    CLOG(TRACE, "Metadata") << "DEBUG: Human JSON contents are:";
    CLOG(TRACE, "Metadata") << m_metadata.dump(2);
    CLOG(TRACE, "Metadata") << "================================";
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

UintVec2 Metadata::getTileCoords() const
{
   auto tile_location = get("tile_location");
  return{ tile_location["x"], tile_location["y"] };
}

json Metadata::get(std::string name, json default_value) const
{
  json result = m_metadata.value(name, default_value);

  if (StringTransforms::hasEnding(name, "_range") && result.is_array())
  {
    result = pick_uniform(static_cast<int>(result[0]), 
                          static_cast<int>(result[1]));
  }

  return result;
}

json Metadata::get(json::json_pointer name, json default_value) const
{
  json result = m_metadata.value(name, default_value);

  if (StringTransforms::hasEnding(name, "_range") && result.is_array())
  {
    result = pick_uniform(static_cast<int>(result[0]),
                          static_cast<int>(result[1]));
  }

  return result;
}

void Metadata::set(std::string name, json value)
{
  m_metadata[name] = value;
}

void Metadata::set(json::json_pointer name, json value)
{
  m_metadata[name] = value;
}

void Metadata::add(Metadata const& other)
{
  json flatData = m_metadata.flatten();

  json flatOtherData = other.m_metadata.flatten();

  for (auto iter = flatOtherData.cbegin(); iter != flatOtherData.cend(); ++iter)
  {
    if (flatData.count(iter.key()) == 0)
    {
      flatData[iter.key()] = iter.value();
    }
  }

  m_metadata = flatData.unflatten();
}
