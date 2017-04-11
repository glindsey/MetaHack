#include "stdafx.h"

#include "services/FileSystemGameRules.h"

#include "game/App.h" // needed for the_lua_instance
#include "utilities/JSONUtils.h"
#include "Service.h"
#include "services/IGraphicViews.h"

// Namespace aliases
namespace fs = boost::filesystem;

FileSystemGameRules::FileSystemGameRules()
{}

FileSystemGameRules::~FileSystemGameRules()
{}

json & FileSystemGameRules::category(std::string name)
{
  if (m_data["categories"].count(name) == 0)
  {
    loadCategory(name);
  }

  return m_data["categories"][name];
}

void FileSystemGameRules::loadCategory(std::string name)
{
  json& category_data = m_data["categories"][name];

  // Look for the various files containing this metadata.
  FileName resource_string = "resources/entity/" + name;
  FileName jsonfile_string = resource_string + ".json";
  FileName luafile_string = resource_string + ".lua";
  fs::path luafile_path = fs::path(luafile_string);
  fs::path jsonfile_path = fs::path(jsonfile_string);

  /// Try to load this Entity's JSON metadata.
  if (fs::exists(jsonfile_path))
  {
    CLOG(INFO, "GameState") << "Loading JSON data for " << name << " category";

    try
    {
      std::ifstream ifs(jsonfile_string);
      category_data << ifs;
    }
    catch (std::exception& e)
    {
      CLOG(FATAL, "GameState") << "Error reading " <<
        jsonfile_string << ": " << e.what();
    }
  }
  else
  {
    CLOG(WARNING, "GameState") << "Can't find " << jsonfile_string;
  }

  /// Check the parent key.
  if (category_data.count("parent") != 0)
  {
    std::string parent_name = category_data["parent"];
    if (parent_name == name)
    {
      CLOG(FATAL, "GameState") << name << " is defined as its own parent. What do you think this is, a Heinlein story?";
    }
    json& parent_data = category(parent_name);

    JSONUtils::addTo(category_data, parent_data);
  }

  // DEBUG: Check for "Human".
  if (name == "Human")
  {
    CLOG(TRACE, "GameState") << "================================";
    CLOG(TRACE, "GameState") << "DEBUG: Human JSON contents are:";
    CLOG(TRACE, "GameState") << category_data.dump(2);
    CLOG(TRACE, "GameState") << "================================";
  }

  /// Try to load and run this Entity's Lua script.
  if (fs::exists(luafile_path))
  {
    CLOG(INFO, "GameState") << "Loading Lua script for " << name;
    the_lua_instance.require(resource_string, true);
  }
  else
  {
    CLOG(WARNING, "GameState") << "Can't find " << luafile_string;
  }

  Service<IGraphicViews>::get().loadViewResourcesFor(name, category_data);
}