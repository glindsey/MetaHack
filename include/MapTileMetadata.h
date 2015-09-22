#ifndef MAPTILEMETADATA_H
#define MAPTILEMETADATA_H

#include <memory>
#include <map>
#include <unordered_map>
#include <string>

class MapTileMetadata
{
public:
  virtual ~MapTileMetadata();

  MapTileMetadata& get(std::string type);

  std::string const& get_pretty_name() const;
  std::string const& get_description() const;

  bool get_flag(std::string key, bool default_value = false) const;
  int get_value(std::string key, int default_value = 0) const;
  std::string get_string(std::string key, std::string default_value = "") const;

private:
  /// Constructor is private.
  MapTileMetadata(std::string type);


  struct Impl;
  std::unique_ptr<Impl> pImpl;

  /// Static collection of MapTileMetadata instances.
  static std::map< std::string, std::unique_ptr<MapTileMetadata> > collection;
};

#endif // MAPTILEMETADATA_H