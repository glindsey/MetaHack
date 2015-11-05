#ifndef METADATA_H
#define METADATA_H

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <SFML/Graphics.hpp>

#include "PropertyDictionary.h"

#define BOOST_FILESYSTEM_NO_DEPRECATED

// Namespace aliases
namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

class Metadata
{
public:
  Metadata(std::string category, std::string type);
  virtual ~Metadata();

  /// Get the name associated with this data.
  std::string const& get_name() const;
  std::string const& get_display_name() const;
  std::string const& get_display_plural() const;
  std::string const& get_description() const;
  sf::Vector2u const& get_tile_coords() const;


protected:
  /// Recursive function that iterates through the tree and prints the values.
  void trace_tree(pt::ptree const* pTree = nullptr, std::string prefix = "");

  /// Get the raw property tree containing metadata.
  pt::ptree const& get_ptree();

  /// Clear the raw property tree. Should only be done after all data needed
  /// has been read from it.
  void clear_ptree();

private:
  /// Category of this metadata (e.g. "maptile", "thing", etc.)
  std::string m_category;

  /// The name associated with this metadata.
  std::string m_name;

  /// The display name.
  std::string m_display_name;

  /// The display plural.
  std::string m_display_plural;

  /// A brief description.
  std::string m_description;

  /// Read-only properties (intrinsics).
  PropertyDictionary m_intrinsics;

  /// Read-write properties. The ones contained here are the defaults
  /// which are copied to a target (Thing, MapTile, etc.) on creation.
  PropertyDictionary m_defaults;

  /// The raw metadata. It is a bit wasteful to save this
  /// independently of other data, so it should probably only be saved
  /// for as long as it is needed.
  pt::ptree m_raw_ptree;

  /// Boolean indicating whether this metadata has graphics associated with it.
  bool m_has_tiles;

  /// Location of this metadata's graphics on the tilesheet.
  sf::Vector2u m_tile_location;
};

#endif // METADATA_H

