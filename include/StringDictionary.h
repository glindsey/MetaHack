#include "stdafx.h"

#include <boost/noncopyable.hpp>

#define BOOST_FILESYSTEM_NO_DEPRECATED

// Namespace aliases
namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

// Using declarations
using StringMap = std::unordered_map<std::string, std::string>;

class StringDictionary : public boost::noncopyable
{
public:
	/// Create a string dictionary for the specified language.
	StringDictionary(std::string language_);

	~StringDictionary();

	/// Add the strings in the specified file to the dictionary.
	/// The dictionary language and ".xml" will be appended to the filename;
	/// for example, StringDictionary("en")::load_file("strings") will try to
	/// load the file named "strings.en.xml".
	/// @param filename_ The name of file to load, _without_ extension.
	/// @return True if the file was successfully loaded and parsed, false otherwise.
	bool load_file(std::string filename_);

	/// Add the specified string to the dictionary.
	/// If the specified ID already exists in the dictionary, it will
	/// be replaced with the new translation.
	/// If the language passed in doesn't match the dictionary language,
	/// this method does nothing and returns false.
	/// @param id_			ID of the string to add.
	/// @param str_			Translated string to associate with that ID.
	/// @param language_	Language of the translated string. Optional;
	///                     the dictionary language is assumed if this
	///                     parameter is not present.
	/// @return True if the string was added, false otherwise.
	bool add(std::string id_, std::string str_, std::string language_);

	/// Get the string corresponding with an ID.
	/// If the ID doesn't exist in the database, it will be returned as the
	/// translated string. (This helps identify missing strings in the
	/// dictionary.)
	/// @param id_	ID of the string to get.
	/// @return The translated string, or the ID if it doesn't exist in the
	///         dictionary.
	std::string const& get(std::string id_) const;

	/// Convenience operator[] for the dictionary.
	std::string const& operator[](std::string id_) const;

protected:


private:
	/// Language that this dictionary is for.
	std::string language;

	/// Data structure mapping string IDs to strings in the specified language.
	StringMap	strings;
};