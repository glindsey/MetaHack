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

struct Metadata::Impl
{
  /// Category of this metadata (e.g. "maptile", "thing", etc.)
  std::string category;

  /// The name associated with this metadata.
  std::string name;

  /// The display name.
  std::string display_name;

  /// The display plural.
  std::string display_plural;

  /// A brief description.
  std::string description;

  /// The raw metadata. It is a bit wasteful to save this
  /// independently of other data, so it should probably only be saved
  /// for as long as it is needed.
  pt::ptree raw_ptree;

  /// Boolean indicating whether this metadata has graphics associated with it.
  bool has_tiles;

  /// Location of this metadata's graphics on the tilesheet.
  sf::Vector2u tile_location;
};

Metadata::Metadata(std::string category, std::string type)
  : pImpl(NEW Impl())
{
  TRACE("Loading metadata for %s::%s...", category.c_str(), type.c_str());

  pImpl->category = category;
  pImpl->name = type;

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
  pt::xml_parser::read_xml(xmlfile_string, pImpl->raw_ptree);

  // DEBUG: Dump the tree using trace.
  this->trace_tree();

  pt::ptree& data = (pImpl->raw_ptree).get_child(pImpl->category);

  // Get the pretty name.
  try
  {
    pImpl->display_name = data.get_child("name").get_value<std::string>("[" + type + "]");
  }
  catch (pt::ptree_bad_path&)
  {
    pImpl->display_name = "[" + type + "]";
  }

  // Get thing's pretty plural, if present. Otherwise add "s" to the normal pretty name.
  try
  {
    pImpl->display_plural = data.get_child("plural").get_value<std::string>(pImpl->display_name + "s");
  }
  catch (pt::ptree_bad_path&)
  {
    pImpl->display_plural = pImpl->display_name + "s";
  }

  // Get thing's description.
  try
  {
    pImpl->description = data.get_child("description").get_value<std::string>("(No description found.)");
  }
  catch (pt::ptree_bad_path&)
  {
    pImpl->description = "(No description found.)";
  }

  if (fs::exists(pngfile_path))
  {
    pImpl->has_tiles = true;
    pImpl->tile_location = TS.load_collection(pngfile_string);
    TRACE("Tiles for %s:%s were placed on the TileSheet at (%u, %u)",
      category.c_str(), type.c_str(), pImpl->tile_location.x, pImpl->tile_location.y);
  }
  else
  {
    TRACE("No tiles found for %s:%s", category.c_str(), type.c_str());
    pImpl->has_tiles = false;
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
  return pImpl->name;
}

std::string const& Metadata::get_display_name() const
{
  return pImpl->display_name;
}

std::string const& Metadata::get_display_plural() const
{
  return pImpl->display_plural;
}

std::string const& Metadata::get_description() const
{
  return pImpl->description;
}

sf::Vector2u const& Metadata::get_tile_coords() const
{
  return pImpl->tile_location;
}

// === PROTECTED METHODS ======================================================

void Metadata::trace_tree(pt::ptree const* pTree, std::string prefix)
{
  if (pTree == nullptr)
  {
    pTree = &(pImpl->raw_ptree);
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
  return pImpl->raw_ptree.get_child(pImpl->category);
}

/// Clear the raw property tree. Should only be done after all data needed
/// has been read from it.
void Metadata::clear_ptree()
{
  pImpl->raw_ptree.clear();
}

