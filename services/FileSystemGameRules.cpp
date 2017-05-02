#include "stdafx.h"

#include "services/FileSystemGameRules.h"

#include "game/App.h" // needed for the_lua_instance
#include "Service.h"
#include "services/IGraphicViews.h"
#include "utilities/JSONUtils.h"
#include "utilities/StringTransforms.h"

// Namespace aliases
namespace fs = boost::filesystem;

FileSystemGameRules::FileSystemGameRules()
{}

FileSystemGameRules::~FileSystemGameRules()
{}

json & FileSystemGameRules::category(std::string name, bool isTemplate)
{
  loadCategoryIfNecessary(name, isTemplate);
  return m_data["categories"][name];
}

void FileSystemGameRules::loadCategoryIfNecessary(std::string name, bool isTemplate)
{
  json& categories = m_data["categories"];

  if (categories.count(name) == 0)
  {
    CLOG(TRACE, "GameState") << "Loading data for " << name << " category...";

    json& categoryData = categories[name];

    // Look for the various files containing this metadata.
    FileName resource_string = (isTemplate ?
                                "resources/entity/templates/" :
                                "resources/entity/") + name;
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

    // *** Load templates ***
    JSONUtils::addIfMissing(categoryData, "templates", json::array());
    json& templatesJson = categoryData["templates"];

    if (templatesJson.is_array())
    {
      for (auto index = 0; index < templatesJson.size(); ++index)
      {
        // Sanitize first.
        std::string tempName = templatesJson[index].get<std::string>();
        tempName = StringTransforms::remove_extra_whitespace_from(tempName);
        templatesJson[index] = tempName;

        if (name == tempName)
        {
          CLOG(FATAL, "GameState") << name << " has itself as a template -- this isn't allowed!";
        }

        loadCategoryIfNecessary(tempName, true);
        json& subcategoryData = categories[tempName];
        JSONUtils::mergeArrays(categoryData["templates"], subcategoryData["templates"]);
        JSONUtils::addTo(categoryData["components"], subcategoryData["components"]);
      }
    }
    else
    {
      CLOG(WARNING, "GameState") << " -- " << name << " has a \"templates\" key, but the value is not an array. Skipping.";
    }

    // Populate the "category" component.
    // (If one was present already in the file, it will be overwritten.)
    componentsJson["category"] = name;

    // DEBUG: Check for "Human".
    //if (name == "Human")
    {
      CLOG(TRACE, "GameState") << "================================";
      CLOG(TRACE, "GameState") << "DEBUG: " << name << " JSON contents are:";
      CLOG(TRACE, "GameState") << categoryData.dump(2);
      CLOG(TRACE, "GameState") << "================================";
    }

    /// Try to load and run this Entity's Lua script.
    if (fs::exists(luafile_path))
    {
      the_lua_instance.require(resource_string, true);
    }

    Service<IGraphicViews>::get().loadViewResourcesFor(name, categoryData);
  }
}