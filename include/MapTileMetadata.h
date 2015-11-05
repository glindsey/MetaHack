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

  /// @todo Make this return a reference, like ThingMetadata does.
  static MapTileMetadata* get(std::string type);

  template<typename T>
  T get_intrinsic(std::string key) const
  {
    PropertyDictionary const& intrinsics = get_intrinsics();
    if (intrinsics.contains(key))
    {
      return get_intrinsics().get<T>(key);
    }
    else
    {
      std::string parent = get_parent();
      if (!parent.empty())
      {
        MapTileMetadata* parent_metadata = get(parent);
        return parent_metadata->get_intrinsic<T>(key);
      }
      else
      {
        return T();
      }
    }
  }

  template<typename T>
  T get_default(std::string key) const
  {
    PropertyDictionary const& defaults = get_defaults();
    if (defaults.contains(key))
    {
      return get_defaults().get<T>(key);
    }
    else
    {
      std::string parent = get_parent();
      if (!parent.empty())
      {
        MapTileMetadata* parent_metadata = get(parent);
        return parent_metadata->get_default<T>(key);
      }
      else
      {
        return T();
      }
    }
  }

private:
  /// Constructor is private; new instances are obtained using get().
  MapTileMetadata(std::string type);

  /// Static collection of MapTileMetadata instances.
  static boost::ptr_unordered_map<std::string, MapTileMetadata> MapTileMetadata::s_collection;
};

#endif // MAPTILEMETADATA_H