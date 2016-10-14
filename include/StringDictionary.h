#include "stdafx.h"

#include <boost/noncopyable.hpp>

// Namespace aliases
namespace pt = boost::property_tree;

// Using declarations
using LanguageMap = std::unordered_map<std::string, std::string>;
using StringMap = std::unordered_map<std::string, LanguageMap>;

class StringDictionary : public boost::noncopyable
{
public:
	StringDictionary();
	~StringDictionary();

protected:


private:
	/// Data structure mapping string IDs to strings in various languages.
	StringMap	strings;
};