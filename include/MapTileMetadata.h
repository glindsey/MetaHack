#ifndef MAPTILEMETADATA_H
#define MAPTILEMETADATA_H

#include <memory>
#include <string>
#include <boost/ptr_container/ptr_unordered_map.hpp>
#include <SFML/Graphics.hpp>

#include "common_types.h"
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
  /// Constructor is private; new instances are obtained using get().
  MapTileMetadata(std::string type);

  /// Map of flags.
  FlagsMap m_flags;

  /// Map of values.
  ValuesMap m_values;

  /// Map of strings.
  StringsMap m_strings;

  /// Static collection of MapTileMetadata instances.
  static boost::ptr_unordered_map<std::string, MapTileMetadata> MapTileMetadata::s_collection;
};

#endif // MAPTILEMETADATA_H