#include "PropertyDictionary.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "common_functions.h"

PropertyDictionary::PropertyDictionary()
{}

PropertyDictionary::~PropertyDictionary()
{}

void PropertyDictionary::populate_from(pt::ptree const& tree, std::string prefix)
{
  // Get each of the defaults.
  for (auto& child_tree : tree)
  {
    std::string key = prefix + child_tree.first;
    std::string value = child_tree.second.get_value<std::string>();
    std::string lowercase_value = boost::algorithm::to_lower_copy(value);
    boost::algorithm::to_lower(key);
   
    boost::trim(key);
    boost::trim(value);

    if (!value.empty())
    {
      // Do some key name fixup; we want to be flexible.
      boost::algorithm::replace_last(key, "_flag", "_f");
      boost::algorithm::replace_last(key, "_string", "_s");
      boost::algorithm::replace_last(key, "_str", "_s");
      boost::algorithm::replace_last(key, "_value", "");
      boost::algorithm::replace_last(key, "_val", "");
      boost::algorithm::replace_last(key, "_v", "");
      
      if (boost::algorithm::ends_with(key, "_f"))
      {
        // Explicitly specified flag.
        boost::algorithm::replace_last(key, "_f", "");
        try
        {
          bool bool_value = boost::lexical_cast<bool>(value);
          set(key, bool_value);
        }
        catch (boost::bad_lexical_cast&)
        {
          set(key, false);
        }
      }
      else if (boost::algorithm::ends_with(key, "_s"))
      {
        // Explicitly specified string.
        boost::algorithm::replace_last(key, "_s", "");
        set(key, value);
      }
      else if (boost::algorithm::ends_with(key, "_v"))
      {
        // Explicitly specified value.
        boost::algorithm::replace_last(key, "_v", "");
        try
        {
          double double_value = boost::lexical_cast<double>(value);
          set(key, double_value);
        }
        catch (boost::bad_lexical_cast&)
        {
          set(key, 0.0);
        }
      }
      else
      {
        // Try to deduce from the string.

        // If the value is surrounded by double quotes, it is always considered
        // to be a string.
        if (strip_quotes(value))
        {
          set(key, value);
        }
        // Lowercase value of "true"/"false"/"yes"/"no" is a boolean.
        else if ((lowercase_value == "true") || (lowercase_value == "yes"))
        {
          set(key, true);
        }
        else if ((lowercase_value == "false") || (lowercase_value == "no"))
        {
          set(key, false);
        }
        else
        {
          try
          {
            // Try to make a double out of the value.
            double double_value = boost::lexical_cast<double>(value);
            set(key, double_value);
          }
          catch (boost::bad_lexical_cast&)
          {
              // Okay, fine, just save it as a string.
              set(key, value);
          } // end try/catch for double
        } // end else (didn't match other test conditions)
      } // end else (didn't have an explicit suffix)
    } // end (value is not empty)

    // Iterate through any children of this key.
    populate_from(child_tree.second, key + ".");
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
