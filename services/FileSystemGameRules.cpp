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

json & FileSystemGameRules::categoryData(std::string name, std::string subType)
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

    std::string type = (subType.empty() ? "category" : subType);

    // Populate the "type" and "full-name" components.
    componentsJson["type"] = type;
    componentsJson["full-name"] = fullName;

    // Populate the "category" component, or if this is a subtype, the 
    // component of that name.
    // (If one was present already in the file, it will be overwritten.)
    componentsJson[type] = name;

    // *** Load templates ***
    if (categoryData.count("templates") != 0)
    {
      loadTemplateComponents(categoryData["templates"], componentsJson);
    }

    // Material is only populated when an Entity is actually created, since
    // it may change on a per-Entity basis.

    // DEBUG: Check for a specific object type
    if (name == "OpenSpace")
    {
      CLOG(TRACE, "GameRules") << "================================";
      CLOG(TRACE, "GameRules") << "DEBUG: " << fullName << " JSON contents are:";
      CLOG(TRACE, "GameRules") << categoryData.dump(2);
      CLOG(TRACE, "GameRules") << "================================";
    }

    /// Try to load and run this Entity's Lua script.
    if (fs::exists(luafile_path))
    {
      GAME.lua().require(resource_string, true);
    }

    if (name == "OpenSpace")
    {
      std::cerr << "BREAK" << std::endl;
    }

    S<IGraphicViews>().loadViewResourcesFor(name, categoryData);
  }
}

void FileSystemGameRules::loadTemplateComponents(json& templates, json& components)
{
  json& categories = m_data["categories"];

  std::string fullName = components["full-name"].get<std::string>();

  if (templates.is_array())
  {
    for (size_t index = 0; index < templates.size(); ++index)
    {
      // Sanitize first.
      std::string templateName = templates[index].get<std::string>();
      templateName = StringTransforms::squishWhitespace(templateName);
      std::string templateFullName = "template." + templateName;
      templates[index] = templateName;

      if (fullName == templateFullName)
      {
        CLOG(FATAL, "GameRules") << fullName << " has itself as a template -- this isn't allowed!";
      }

      loadCategoryIfNecessary(templateName, "template");
      json& subcategoryData = categories[templateFullName];
      JSONUtils::mergeArrays(templates, subcategoryData["templates"]);
      JSONUtils::addMissingKeys(components, subcategoryData["components"]);
    }
  }
  else
  {
    CLOG(WARNING, "GameRules") << " -- " << fullName << " has a \"templates\" key, but the value is not an array. Skipping.";
  }
}