#include "PropertyDictionary.h"

#include <boost/algorithm/string.hpp>

PropertyDictionary::PropertyDictionary()
{}

PropertyDictionary::~PropertyDictionary()
{}

void PropertyDictionary::populate_from(pt::ptree const& tree)
{
  // Get each of the defaults.
  for (auto& child_tree : tree)
  {
    std::string key = child_tree.first;
    boost::algorithm::to_lower(key);

    // Do some key name fixup; we want to be flexible.
    boost::algorithm::replace_last(key, "_flag", "_f");
    boost::algorithm::replace_last(key, "_string", "_s");
    boost::algorithm::replace_last(key, "_str", "_s");
    boost::algorithm::replace_last(key, "_value", "");
    boost::algorithm::replace_last(key, "_val", "");
    boost::algorithm::replace_last(key, "_v", "");

    if (boost::algorithm::ends_with(key, "_f"))
    {
      boost::algorithm::replace_last(key, "_f", "");
      bool value = child_tree.second.get_value<bool>(false);
      set(key, value);
    }
    else if (boost::algorithm::ends_with(key, "_s"))
    {
      boost::algorithm::replace_last(key, "_s", "");
      std::string value = child_tree.second.get_value<std::string>("");
      set(key, value);
    }
    else
    {
      double value = child_tree.second.get_value<double>(0.0);
      set(key, value);
    }
  }
}

bool PropertyDictionary::contains(std::string key) const
{
  return (m_dictionary.count(key) != 0);
}


AnyMap& PropertyDictionary::get_dictionary()
{
  return m_dictionary;
}
