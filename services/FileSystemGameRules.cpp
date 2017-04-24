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
  json& categoryData = m_data["categories"][name];

  // Look for the various files containing this metadata.
  FileName resource_string = "resources/entity/" + name;
  FileName jsonfile_string = resource_string + ".json";
  fs::path jsonfile_path = fs::path(jsonfile_string);
  FileName luafile_string = resource_string + ".lua";
  fs::path luafile_path = fs::path(luafile_string);

  /// Try to load this Entity's JSON metadata.
  if (fs::exists(jsonfile_path))
  {
    CLOG(INFO, "GameState") << "Loading data for " << name << " category";

    try
    {
      std::ifstream ifs(jsonfile_string);
      categoryData << ifs;
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

  // If there's no "components" key, create one. 
  // (There should be, but do it just to be sure.)
  JSONUtils::addIfMissing(categoryData, "components", json::object());
  json& componentsJson = categoryData["components"];

  // *** Get templates ***
  if (categoryData.count("templates") != 0)
  {
    json& templatesJson = categoryData["templates"];
    if (templatesJson.is_array())
    {
      for (auto citer = templatesJson.cbegin(); citer != templatesJson.cend(); ++citer)
      {
        CLOG(INFO, "GameState") << " -- Adding " << citer.value() << " template";
        json templateData = json::object();
        loadTemplateInto(templateData, citer.value());
        /// @todo Concatenate template arrays.
        JSONUtils::mergeArrays(categoryData["templates"], templateData["templates"]);
        JSONUtils::addTo(categoryData["components"], templateData["components"]);
      }
    }
    else
    {
      CLOG(WARNING, "GameState") << " -- " << name << " has a \"templates\" key, but the value is not an array. Skipping.";
    }
  }

  // Populate the "category" component.
  // (If one was present already in the file, it will be overwritten.)
  componentsJson["category"] = name;

  // DEBUG: Check for "Human".
  if (name == "Human")
  {
    CLOG(TRACE, "GameState") << "================================";
    CLOG(TRACE, "GameState") << "DEBUG: Human JSON contents are:";
    CLOG(TRACE, "GameState") << categoryData.dump(2);
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

  Service<IGraphicViews>::get().loadViewResourcesFor(name, categoryData);
}

void FileSystemGameRules::loadTemplateInto(json& templateData, std::string name)
{
  // Look for the various files containing this metadata.
  FileName resource_string = "resources/entity/templates/" + name;
  FileName jsonfile_string = resource_string + ".json";
  fs::path jsonfile_path = fs::path(jsonfile_string);
  FileName luafile_string = resource_string + ".lua";
  fs::path luafile_path = fs::path(luafile_string);

  /// Try to load this Entity's JSON metadata.
  if (fs::exists(jsonfile_path))
  {
    try
    {
      std::ifstream ifs(jsonfile_string);
      templateData << ifs;
    }
    catch (std::exception& e)
    {
      CLOG(FATAL, "GameState") << "  ---- Error reading " <<
        jsonfile_string << ": " << e.what();
    }
  }
  else
  {
    CLOG(WARNING, "GameState") << "  ---- Can't find " << jsonfile_string << ". Skipping.";
  }

  // *** See if this template refers to its OWN templates ***
  if (templateData.count("templates") != 0)
  {
    json& templatesJson = templateData["templates"];
    if (templatesJson.is_array())
    {
      for (auto citer = templatesJson.cbegin(); citer != templatesJson.cend(); ++citer)
      {
        json templateData2 = json::object();
        CLOG(INFO, "GameState") << " -- Adding " << citer.value() << " template";
        loadTemplateInto(templateData2, citer.value());

        JSONUtils::mergeArrays(templateData["templates"], templateData2["templates"]);
        JSONUtils::addTo(templateData["components"], templateData2["components"]);
      }
    }
    else
    {
      CLOG(WARNING, "GameState") << " -- " << name << " has a \"templates\" key, but the value is not an array. Skipping.";
    }
  }

  /// Try to load and run this template's Lua script.
  if (fs::exists(luafile_path))
  {
    CLOG(INFO, "GameState") << " -- Loading Lua script for " << name;
    the_lua_instance.require(resource_string, true);
  }
  else
  {
    CLOG(WARNING, "GameState") << " -- Can't find " << luafile_string << ". Skipping.";
  }
}
