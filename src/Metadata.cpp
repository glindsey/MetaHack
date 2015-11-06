#include "Metadata.h"

#include <boost/algorithm/string.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <memory>

#include "common_functions.h"
#include "common_types.h"

#include "App.h"
#include "ErrorHandler.h"
#include "Exceptions.h"
#include "Lua.h"
#include "Tilesheet.h"

Metadata::Metadata(std::string category, std::string type)
{
  TRACE("Loading metadata for %s!%s...", category.c_str(), type.c_str());

  m_category = category;
  m_type = type;

  // Look for the various files containing this metadata.
  std::string xmlfile_string = "resources/" + category + "s/" + type + ".xml";
  fs::path xmlfile_path = fs::path(xmlfile_string);
  std::string pngfile_string = "resources/" + category + "s/" + type + ".png";
  fs::path pngfile_path = fs::path(pngfile_string);
  std::string luafile_string = "resources/" + category + "s/" + type + ".lua";
  fs::path luafile_path = fs::path(luafile_string);

  if (!fs::exists(xmlfile_path))
  {
    FATAL_ERROR("Can't find %s", xmlfile_string.c_str());
  }

  /// Load file.
  pt::xml_parser::read_xml(xmlfile_string, m_raw_ptree);

  // DEBUG: Dump the tree using trace.
  //this->trace_tree();

  pt::ptree& data = (m_raw_ptree).get_child(m_category);

  // Get thing's parent.
  if (data.count("parent") == 0)
  {
    m_parent = "";
  }
  else
  {
    m_parent = data.get_child("parent").get_value<std::string>("");
    strip_quotes(m_parent);
  }

  // Get the pretty name.
  if (data.count("name") == 0)
  {
    m_display_name = "[" + type + "]";
  }
  else
  {
    m_display_name = data.get_child("name").get_value<std::string>("[" + type + "]");
    boost::trim(m_display_name);
    strip_quotes(m_display_name);
  }

  // Get thing's pretty plural, if present. Otherwise add "s" to the normal pretty name.
  if (data.count("plural") == 0)
  {
    m_display_plural = m_display_name + "s";
  }
  else
  {
    m_display_plural = data.get_child("plural").get_value<std::string>(m_display_name + "s");
    boost::trim(m_display_plural);
    strip_quotes(m_display_plural);
  }

  // Get thing's description.
  if (data.count("description") == 0)
  {
    m_description = "(No description found.)";
  }
  else
  {
    m_description = data.get_child("description").get_value<std::string>("(No description found.)");
    boost::trim(m_description);
    strip_quotes(m_description);
  }

  // Look for intrinsics, defaults sections. Both must be present in any metadata file.
  pt::ptree intrinsics_tree;
  if (data.count("intrinsics") == 0)
  {
    FATAL_ERROR("Metadata file for %s!%s doesn't have an \"intrinsics\" section", m_category.c_str(), m_type.c_str());
  }
  else
  {
    intrinsics_tree = data.get_child("intrinsics");
  }

  pt::ptree defaults_tree;
  if (data.count("defaults") == 0)
  {
    FATAL_ERROR("Metadata file for %s!%s doesn't have a \"defaults\" section", m_category.c_str(), m_type.c_str());
  }
  else
  {
    defaults_tree = data.get_child("defaults");
  }

  // Populate intrinsics dictionary.
  m_intrinsics.populate_from(intrinsics_tree);

  // Populate defaults dictionary.
  m_defaults.populate_from(defaults_tree);

  if (fs::exists(pngfile_path))
  {
    m_has_tiles = true;
    m_tile_location = TS.load_collection(pngfile_string);
    TRACE("Tiles for %s!%s were placed on the TileSheet at (%u, %u)",
      category.c_str(), type.c_str(), m_tile_location.x, m_tile_location.y);
  }
  else
  {
    TRACE("No tiles found for %s!%s", category.c_str(), type.c_str());
    m_has_tiles = false;
  }

  /// Now try to load and run a Lua script for this Thing if one exists.
  if (fs::exists(luafile_path))
  {
    TRACE("Loading Lua script for %s!%s...", category.c_str(), type.c_str());

    the_lua_instance.do_file(luafile_string);
  }
}

Metadata::~Metadata()
{

}

/// Get the name associated with this data.
std::string const& Metadata::get_name() const
{
  return m_type;
}

std::string const& Metadata::get_parent() const
{
  return m_parent;
}

std::string const& Metadata::get_display_name() const
{
  return m_display_name;
}

std::string const& Metadata::get_display_plural() const
{
  return m_display_plural;
}

std::string const& Metadata::get_description() const
{
  return m_description;
}

sf::Vector2u const& Metadata::get_tile_coords() const
{
  return m_tile_location;
}

PropertyDictionary const& Metadata::get_defaults() const
{
  return m_defaults;
}

PropertyDictionary const& Metadata::get_intrinsics() const
{
  return m_intrinsics;
}

// === PROTECTED METHODS ======================================================

void Metadata::trace_tree(pt::ptree const* pTree, std::string prefix)
{
  if (pTree == nullptr)
  {
    pTree = &(m_raw_ptree);
  }

  pt::ptree::const_iterator end = pTree->end();
  for (pt::ptree::const_iterator it = pTree->begin(); it != end; ++it)
  {
    std::string key = prefix + (it->first);
    boost::to_lower(key);
    std::string value = it->second.get_value<std::string>();
    boost::trim(key);
    boost::trim(value);
    if (!value.empty())
    {
      TRACE("%s = \"%s\"", key.c_str(), value.c_str());
    }
    trace_tree(&(it->second), key + ".");
  }
}

/// Get the raw property tree containing metadata.
pt::ptree const& Metadata::get_ptree()
{
  return m_raw_ptree.get_child(m_category);
}

/// Clear the raw property tree. Should only be done after all data needed
/// has been read from it.
void Metadata::clear_ptree()
{
  m_raw_ptree.clear();
}

