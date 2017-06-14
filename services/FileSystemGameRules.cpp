#include "stdafx.h"

#include "services/FileSystemGameRules.h"

#include "game/GameState.h" // needed for LUA
#include "lua/LuaObject.h"
#include "services/Service.h"
#include "services/IGraphicViews.h"
#include "utilities/JSONUtils.h"
#include "utilities/StringTransforms.h"

// Namespace aliases
namespace fs = boost::filesystem;

FileSystemGameRules::FileSystemGameRules()
{}

FileSystemGameRules::~FileSystemGameRules()
{}

json & FileSystemGameRules::category(std::string name, std::string subType)
{
  std::string fullName = subType.empty() ? name : subType + "." + name;
  loadCategoryIfNecessary(name, subType);
  return m_data["categories"][fullName];
}

void FileSystemGameRules::loadCategoryIfNecessary(std::string name, std::string subType)
{
  json& categories = m_data["categories"];
  std::string fullName = subType.empty() ? name : subType + "." + name;

  if (categories.count(fullName) == 0)
  {
    CLOG(TRACE, "GameRules") << "Loading data for " << fullName << " category...";

    json& categoryData = categories[fullName];

    // Look for the various files containing this metadata.
    FileName resource_string = "resources/entity/" +
                               (subType.empty() ? "" : subType + "/") + name;

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
        categoryData << ifs;
      }
      catch (std::exception& e)
      {
        CLOG(FATAL, "GameRules") << "Error reading " <<
          jsonfile_string << ": " << e.what();
      }
    }
    else
    {
      CLOG(FATAL, "GameRules") << "Can't find " << jsonfile_string;
    }

    // If there's no "components" key, create one. 
    // (There should be, but do it just to be sure.)
    JSONUtils::addIfMissing(categoryData, "components", json::object());
    json& componentsJson = categoryData["components"];

    // Populate the "category" component.
    // (If one was present already in the file, it will be overwritten.)
    componentsJson["category"] = name;

    // *** Load templates ***
    if (categoryData.count("templates") != 0)
    {
      loadTemplateComponents(categoryData["templates"], componentsJson);
    }

    // Material is only populated when an Entity is actually created, since
    // it may change on a per-Entity basis.

    // DEBUG: Check for "Human".
    //if (name == "Human")
    //{
    //  CLOG(TRACE, "GameRules") << "================================";
    //  CLOG(TRACE, "GameRules") << "DEBUG: " << fullName << " JSON contents are:";
    //  CLOG(TRACE, "GameRules") << categoryData.dump(2);
    //  CLOG(TRACE, "GameRules") << "================================";
    //}

    /// Try to load and run this Entity's Lua script.
    if (fs::exists(luafile_path))
    {
      GAME.lua().require(resource_string, true);
    }

    S<IGraphicViews>().loadViewResourcesFor(name, categoryData);
  }
}

void FileSystemGameRules::loadTemplateComponents(json& templates, json& components)
{
  json& categories = m_data["categories"];

  std::string name = components["category"].get<std::string>();

  if (templates.is_array())
  {
    for (auto index = 0; index < templates.size(); ++index)
    {
      // Sanitize first.
      std::string tempName = templates[index].get<std::string>();
      tempName = StringTransforms::squishWhitespace(tempName);
      templates[index] = tempName;

      if (name == tempName)
      {
        CLOG(FATAL, "GameRules") << name << " has itself as a template -- this isn't allowed!";
      }

      loadCategoryIfNecessary(tempName, "templates");
      json& subcategoryData = categories["templates." + tempName];
      JSONUtils::mergeArrays(templates, subcategoryData["templates"]);
      JSONUtils::addMissingKeys(components, subcategoryData["components"]);
    }
  }
  else
  {
    CLOG(WARNING, "GameRules") << " -- " << name << " has a \"templates\" key, but the value is not an array. Skipping.";
  }
}