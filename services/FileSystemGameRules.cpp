#include "stdafx.h"

#include "services/FileSystemGameRules.h"

#include "config/Paths.h"
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

json & FileSystemGameRules::categoryData(std::string name)
{
  bool hasSubtype = (name.find('.') != std::string::npos);

  loadCategoryIfNecessary(name);
  return m_data[name];
}

void FileSystemGameRules::loadCategoryIfNecessary(std::string name)
{
  StringPair namePair = StringTransforms::splitName(name);

  if (m_data.count(name) == 0)
  {
    CLOG(TRACE, "GameRules") << "Loading data for " << name << " category...";

    json& categoryData = m_data[name];

    // Look for the various files containing this metadata.
    std::string resourcesPath = Config::paths().resources();
    FileName resource_string = "entity/" +
                               (namePair.first.empty() ? "" : namePair.first + "/") + namePair.second;

    FileName jsonfile_string = resourcesPath + "/" + resource_string + ".json";
    fs::path jsonfile_path = fs::path(jsonfile_string);
    FileName luafile_string = resourcesPath + "/" + resource_string + ".lua";
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
    json blankKey;
    JSONUtils::addIfMissing(categoryData, "components", blankKey);
    json& componentsJson = categoryData["components"];

    std::string groupType = (namePair.first.empty() ? "category" : namePair.first);

    // Populate components used for identification.
    componentsJson["type"] = groupType;
    componentsJson[groupType] = namePair.second;
    componentsJson["full-name"] = name;

    // *** Load templates ***
    if (categoryData.count("templates") != 0)
    {
      loadTemplateComponentsFor(name);
    }

    // Material is only populated when an Entity is actually created, since
    // it may change on a per-Entity basis.

    // DEBUG: Check for a specific object type
    if (name == "OpenSpace")
    {
      CLOG(TRACE, "GameRules") << "================================";
      CLOG(TRACE, "GameRules") << "DEBUG: " << name << " JSON contents are:";
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

    S<IGraphicViews>().loadViewResourcesFor(name);
  }
}

void FileSystemGameRules::loadTemplateComponentsFor(std::string name)
{
  StringPair namePair = StringTransforms::splitName(name);

  auto& categoryData = m_data[name];

  auto& components = categoryData["components"];
  auto& templates = categoryData["templates"];

  if (templates.is_array())
  {
    for (size_t index = 0; index < templates.size(); ++index)
    {
      // Sanitize first.
      std::string templateName = templates[index].get<std::string>();
      templateName = StringTransforms::squishWhitespace(templateName);
      std::string templateFullName = "template." + templateName;
      templates[index] = templateName;

      if (name == templateFullName)
      {
        CLOG(FATAL, "GameRules") << name << " has itself as a template -- this isn't allowed!";
      }

      loadCategoryIfNecessary(templateFullName);
      json& subcategoryData = m_data[templateFullName];
      JSONUtils::mergeArrays(templates, subcategoryData["templates"]);
      JSONUtils::addMissingKeys(components, subcategoryData["components"]);
    }
  }
  else
  {
    CLOG(WARNING, "GameRules") << " -- " << name << " has a \"templates\" key, but the value is not an array. Skipping.";
  }
}
