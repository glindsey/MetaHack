#ifndef MAPTILEMETADATA_H
#define MAPTILEMETADATA_H

#include <memory>
#include <string>
#include <boost/ptr_container/ptr_unordered_map.hpp>
#include <SFML/Graphics.hpp>

#include "Metadata.h"

class MapTileMetadata : public Metadata
{
public:
  virtual ~MapTileMetadata();

  static MapTileMetadata* get(std::string type);

  bool get_flag(std::string key, bool default_value = false) const;
  int get_value(std::string key, int default_value = 0) const;
  std::string get_string(std::string key, std::string default_value = "") const;

private:
  /// Constructor is private.
  MapTileMetadata(std::string type);


  struct Impl;
  std::unique_ptr<Impl> pImpl;

  /// Static collection of MapTileMetadata instances.
  static boost::ptr_unordered_map<std::string, MapTileMetadata> MapTileMetadata::collection;
};

#endif // MAPTILEMETADATA_H