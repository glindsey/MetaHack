#ifndef METADATA_H
#define METADATA_H

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <SFML/Graphics.hpp>

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
  struct Impl;
  std::unique_ptr<Impl> pImpl;
};

#endif // METADATA_H

