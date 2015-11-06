#include "PropertyDictionary.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include "common_functions.h"
#include "ErrorHandler.h"
#include "IntegerRange.h"

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
      if (boost::algorithm::ends_with(key, "_flag")) boost::algorithm::erase_tail(key, 3);
      if (boost::algorithm::ends_with(key, "_string")) boost::algorithm::erase_tail(key, 5);
      if (boost::algorithm::ends_with(key, "_str")) boost::algorithm::erase_tail(key, 2);
      if (boost::algorithm::ends_with(key, "_value")) boost::algorithm::erase_tail(key, 4);
      if (boost::algorithm::ends_with(key, "_val")) boost::algorithm::erase_tail(key, 2);
      
      if (boost::algorithm::ends_with(key, "_f"))
      {
        // Explicitly specified flag.
        boost::algorithm::erase_tail(key, 2);
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
        boost::algorithm::erase_tail(key, 2);
        set(key, value);
      }
      else if (boost::algorithm::ends_with(key, "_v"))
      {
        // Explicitly specified value.
        boost::algorithm::erase_tail(key, 2);
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
          boost::regex range_regex("(-?[0-9]+)\\s*to\\s*(-?[0-9]+)\\s*([Uu]?).*");
          boost::smatch str_matches;

          if (boost::regex_match(value, str_matches, range_regex))
          {
            std::string start_string = str_matches[1];
            std::string end_string = str_matches[2];
            std::string uniform_string = str_matches[3];

            int start = boost::lexical_cast<int>(start_string);
            int end = boost::lexical_cast<int>(end_string);
            bool uniform = !uniform_string.empty();

            TRACE("Hey, we found an IntegerRange (%s): start = %d, end = %d, uniform = %s", key.c_str(), start, end, (uniform ? "true" : "false"));

            IntegerRange range(start, end, uniform);
            set(key, range);
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
          } // end else (wasn't a range)
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
