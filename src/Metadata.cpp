#include "Metadata.h"

#include <boost/algorithm/string.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <memory>

#include "common_types.h"

#include "App.h"
#include "ErrorHandler.h"
#include "Exceptions.h"
#include "Lua.h"
#include "Tilesheet.h"

Metadata::Metadata(std::string category, std::string type)
{
  TRACE("Loading metadata for %s::%s...", category.c_str(), type.c_str());

  m_category = category;
  m_name = type;

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
    //throw ExceptionMissingFile("XML", "Thing", type);
  }

  /// Load file.
  pt::xml_parser::read_xml(xmlfile_string, m_raw_ptree);

  // DEBUG: Dump the tree using trace.
  this->trace_tree();

  pt::ptree& data = (m_raw_ptree).get_child(m_category);

  // Get the pretty name.
  try
  {
    m_display_name = data.get_child("name").get_value<std::string>("[" + type + "]");
  }
  catch (pt::ptree_bad_path&)
  {
    m_display_name = "[" + type + "]";
  }

  // Get thing's pretty plural, if present. Otherwise add "s" to the normal pretty name.
  try
  {
    m_display_plural = data.get_child("plural").get_value<std::string>(m_display_name + "s");
  }
  catch (pt::ptree_bad_path&)
  {
    m_display_plural = m_display_name + "s";
  }

  // Get thing's description.
  try
  {
    m_description = data.get_child("description").get_value<std::string>("(No description found.)");
  }
  catch (pt::ptree_bad_path&)
  {
    m_description = "(No description found.)";
  }

#if 0
  // Look for intrinsics, properties sections. Both must be present for any Thing.
  pt::ptree intrinsics_tree = data.get_child("intrinsics");
  pt::ptree properties_tree = data.get_child("properties");
  try
  {
    intrinsics_tree = data.get_child("intrinsics");
    properties_tree = data.get_child("properties");
  }
  catch (pt::ptree_bad_path& p)
  {
    FATAL_ERROR(p.what());
  }

  // Get intrinsic flags.
  if (intrinsics_tree.count("flags") != 0)
  {
    for (auto& child_tree : intrinsics_tree.get_child("flags"))
    {
      std::string key = child_tree.first;
      boost::algorithm::to_lower(key);
      bool value = child_tree.second.get_value<bool>(false);

      m_intrinsic_flags[key] = value;
    }
  }

  // Get intrinsic values.
  if (intrinsics_tree.count("values") != 0)
  {
    for (auto& child_tree : intrinsics_tree.get_child("values"))
    {
      std::string key = child_tree.first;
      boost::algorithm::to_lower(key);
      int value = child_tree.second.get_value<int>(0);

      m_intrinsic_values[key] = value;
    }
  }

  // Get intrinsic strings.
  if (intrinsics_tree.count("strings") != 0)
  {
    for (auto& child_tree : intrinsics_tree.get_child("strings"))
    {
      std::string key = child_tree.first;
      boost::algorithm::to_lower(key);
      std::string value = child_tree.second.get_value<std::string>("");

      m_intrinsic_strings[key] = value;
    }
  }

  // Get default property flags.
  if (properties_tree.count("flags") != 0)
  {
    for (auto& child_tree : properties_tree.get_child("flags"))
    {
      std::string key = child_tree.first;
      boost::algorithm::to_lower(key);
      bool value = child_tree.second.get_value<bool>(false);

      m_default_flags[key] = value;
    }
  }

  // Get default property values.
  if (properties_tree.count("values") != 0)
  {
    for (auto& child_tree : properties_tree.get_child("values"))
    {
      std::string key = child_tree.first;
      boost::algorithm::to_lower(key);
      int value = child_tree.second.get_value<int>(0);

      m_default_values[key] = value;
    }
  }

  // Get default property strings.
  if (properties_tree.count("strings") != 0)
  {
    for (auto& child_tree : properties_tree.get_child("strings"))
    {
      std::string key = child_tree.first;
      boost::algorithm::to_lower(key);
      std::string value = child_tree.second.get_value<std::string>("");

      m_default_strings[key] = value;
    }
  }
#endif

  if (fs::exists(pngfile_path))
  {
    m_has_tiles = true;
    m_tile_location = TS.load_collection(pngfile_string);
    TRACE("Tiles for %s:%s were placed on the TileSheet at (%u, %u)",
      category.c_str(), type.c_str(), m_tile_location.x, m_tile_location.y);
  }
  else
  {
    TRACE("No tiles found for %s:%s", category.c_str(), type.c_str());
    m_has_tiles = false;
  }

  /// Now try to load and run a Lua script for this Thing if one exists.
  if (fs::exists(luafile_path))
  {
    TRACE("Loading Lua script for %s:%s...", category.c_str(), type.c_str());

    the_lua_instance.do_file(luafile_string);
  }
}

Metadata::~Metadata()
{

}

/// Get the name associated with this data.
std::string const& Metadata::get_name() const
{
  return m_name;
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
    std::string name = prefix + (it->first);
    boost::to_lower(name);
    std::string value = it->second.get_value<std::string>();
    boost::trim(value);
    if (!value.empty())
    {
      TRACE("%s = \"%s\"", name.c_str(), value.c_str());
    }
    trace_tree(&(it->second), name + ".");
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

