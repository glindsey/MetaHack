#include "stdafx.h"

/// Interface for a localized string provider.
class IStringDictionary
{
public:
  /// Return a Null Object that implements this interface.
  static IStringDictionary* getNull();

  /// Add the strings in the specified file to the dictionary.
  /// The extension ".json" will be appended to the filename.
  /// @param filename_ The name of file to load, _without_ extension.
  /// @return True if the file was successfully loaded and parsed, false otherwise.
  virtual bool load_file(std::string filename_) = 0;

  /// Add the specified string to the dictionary.
  /// If the specified ID already exists in the dictionary, it will
  /// be replaced with the new translation.
  /// If the language passed in doesn't match the dictionary language,
  /// this method does nothing and returns false.
  /// @param id_			ID of the string to add.
  /// @param str_			Translated string to associate with that ID.
  /// @return True if the string was added, false otherwise.
  virtual bool add(std::string id_, std::string str_) = 0;

  /// Get the string corresponding with an ID.
  /// If the ID doesn't exist in the database, it will be returned as the
  /// translated string. (This helps identify missing strings in the
  /// dictionary.)
  /// @param id_	ID of the string to get.
  /// @return The translated string, or the ID if it doesn't exist in the
  ///         dictionary.
  virtual std::string const& get(std::string id_) const = 0;

  /// Clears out the dictionary.
  virtual void clear() = 0;

  /// Convenience operator[] for the dictionary.
  std::string const& operator[](std::string id_) const
  {
    return get(id_);
  }
};