#ifndef THINGMETADATA_H
#define THINGMETADATA_H

#include <memory>
#include <map>
#include <unordered_map>
#include <string>

using FlagsMap = std::map < std::string, bool > ;
using ValuesMap = std::map < std::string, int > ;
using StringsMap = std::map < std::string, std::string > ;

class ThingMetadata
{
public:
  ThingMetadata(std::string type);
  virtual ~ThingMetadata();

  std::string const& get_pretty_name() const;
  std::string const& get_pretty_plural() const;
  std::string const& get_description() const;
  std::string const& get_parent() const;

  bool get_intrinsic_flag(std::string key, bool default_value = false) const;
  int get_intrinsic_value(std::string key, int default_value = 0) const;
  std::string get_intrinsic_string(std::string key, std::string default_value = "") const;

  FlagsMap const& get_intrinsic_flags() const;
  ValuesMap const& get_intrinsic_values() const;
  StringsMap const& get_intrinsic_strings() const;

  bool get_default_flag(std::string key, bool default_value = false) const;
  int get_default_value(std::string key, int default_value = 0) const;
  std::string get_default_string(std::string key, std::string default_value = "") const;

  FlagsMap const& get_default_flags() const;
  ValuesMap const& get_default_values() const;
  StringsMap const& get_default_strings() const;

private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;
};

#endif // THINGMETADATA_H