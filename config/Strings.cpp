#include "config/Strings.h"

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "types/common.h"

#define BOOST_FILESYSTEM_NO_DEPRECATED

namespace Config
{
  // Namespace aliases
  namespace fs = boost::filesystem;
  namespace pt = boost::property_tree;

  Strings::Strings(std::string filename_)
  {
    if (!filename_.empty())
    {
      loadFile(filename_);
    }
  }

  Strings::~Strings()
  {}

  bool Strings::loadFile(std::string filename_)
  {
    // Look for the file requested.
    FileName filename = filename_ + ".json";

    /// Try to load the requested file.
    if (fs::exists(filename))
    {
      CLOG(INFO, "Strings") << "Loading " << filename;

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
        CLOG(WARNING, "Strings") << "Unable to parse JSON in " << filename << "; translations not loaded";
        return false;
      }

      CLOG(TRACE, "Strings") << "Loaded translations in " << filename;
      return true;
    }
    else
    {
      CLOG(WARNING, "Strings") << filename << " is missing; translations not loaded";
      return false;
    }
  }

  bool Strings::add(std::string id_, std::string str_)
  {
    CLOG(TRACE, "Strings") << "Adding \"" << id_ << "\" = \"" << str_ << "\"";

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

  void Strings::clear()
  {
    strings.clear();
  }

  bool Strings::contains(std::string id_) const
  {
    return (strings.count(id_) != 0);
  }

  std::string Strings::get(std::string id_) const
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

  Strings& strings()
  {
    static std::unique_ptr<Strings> ptr;

    if (!ptr)
    {
      ptr.reset(new Strings());
    }

    return *ptr;
  }
} // end namespace Config
