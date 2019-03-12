#pragma once

#include <boost/noncopyable.hpp>
#include <string>
#include <unordered_map>

namespace Config
{
  // Using declarations
  using StringMap = std::unordered_map<std::string, std::string>;

  class Strings : public boost::noncopyable
  {
  public:
    virtual ~Strings();

    /// Get instance reference.
    friend Strings& strings();

    /// Add the strings in the specified file to the dictionary.
    /// The extension ".json" will be appended to the filename.
    /// @param filename_ The name of file to load, _without_ extension.
    /// @return True if the file was successfully loaded and parsed, false otherwise.
    bool loadFile(std::string filename_);

    /// Add the specified string to the dictionary.
    /// If the specified ID already exists in the dictionary, it will
    /// be replaced with the new translation.
    /// If the language passed in doesn't match the dictionary language,
    /// this method does nothing and returns false.
    /// @param id_      ID of the string to add.
    /// @param str_     Translated string to associate with that ID.
    /// @return True if the string was added, false otherwise.
    bool add(std::string id_, std::string str_);

    /// Clears out the dictionary.
    void clear();

    bool contains(std::string id_) const;

    /// Get the string corresponding with an ID.
    /// If the ID doesn't exist in the database, it will be returned as the
    /// translated string. (This helps identify missing strings in the
    /// dictionary.)
    /// @param id_  ID of the string to get.
    /// @return The translated string, or the ID if it doesn't exist in the
    ///         dictionary.
    std::string get(std::string id_) const;

    /// Convenience operator[] for the dictionary.
    std::string operator[](std::string id_) const
    {
      return get(id_);
    }

  protected:
    /// Create a string dictionary, and optionally populate it from a file.
    /// @filename_ JSON file to populate the dictionary from, minus the
    ///            extension.
    ///            If not present, the dictionary starts out empty.
    Strings(std::string filename_ = "");

  private:

    /// Data structure mapping string IDs to strings.
    StringMap strings;
  };

  Strings& strings();
} // end namespace Config
