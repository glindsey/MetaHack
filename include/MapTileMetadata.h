#ifndef MAPTILEMETADATA_H
#define MAPTILEMETADATA_H

#include <string>

#include "common_types.h"
#include "Metadata.h"

// Forward declaration
class MetadataCollection;

class MapTileMetadata : public Metadata
{
  friend class MetadataCollection;

public:
  virtual ~MapTileMetadata();

private:
  /// Constructor is private; new instances are obtained using the get() method
  /// of the associated collection.
  MapTileMetadata(MetadataCollection& collection, std::string type);
};

#endif // MAPTILEMETADATA_H