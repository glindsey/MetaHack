#include "stdafx.h"

#include "ErrorHandler.h"
#include "StringDictionary.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#define BOOST_FILESYSTEM_NO_DEPRECATED

// Namespace aliases
namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

StringDictionary::StringDictionary(std::string filename_)
{
  // Register the logger.
  SET_UP_LOGGER("StringDictionary", false);

  if (!filename_.empty())
  {
    load_file(filename_);
  }
}

StringDictionary::~StringDictionary()
{}

bool StringDictionary::load_file(std::string filename_)
{
  // Look for the file requested.
  FileName filename = filename_ + ".json";

  /// Try to load the requested file.
  if (fs::exists(filename))
  {
    CLOG(INFO, "StringDictionary") << "Loading " << filename;

    pt::ptree file_contents;
    try
    {
      pt::read_json(filename, file_contents);

      for (auto& value : file_contents)
      {
        add(value.first, value.second.data());
      }
    }
    catch (pt::json_parser_error&)
    {
      CLOG(WARNING, "StringDictionary") << "Unable to parse JSON in " << filename << "; translations not loaded";
      return false;
    }

    CLOG(TRACE, "StringDictionary") << "Loaded translations in " << filename;
    return true;
  }
  else
  {
    CLOG(WARNING, "StringDictionary") << filename << " is missing; translations not loaded";
    return false;
  }
}

bool StringDictionary::add(std::string id_, std::string str_)
{
  CLOG(TRACE, "StringDictionary") << "Adding \"" << id_ << "\" = \"" << str_ << "\"";

  if (strings.count(id_) == 0)
  {
    strings.insert(std::make_pair(id_, str_));
  }
  else
  {
    strings[id_] = str_;
  }
  return true;
}

void StringDictionary::clear()
{
  strings.clear();
}

std::string const& StringDictionary::get(std::string id_) const
{
  if (strings.count(id_) != 0)
  {
    return strings.at(id_);
  }
  else
  {
    return id_;
  }
}