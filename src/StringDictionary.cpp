#include "stdafx.h"

#include "StringDictionary.h"

StringDictionary::StringDictionary(std::string language_)
    :
    language(language_)
{
}

StringDictionary::~StringDictionary()
{
}

bool StringDictionary::load_file(std::string filename_)
{
    // Look for the file requested.
    FileName filename = filename_ + "." + language + ".xml";

    /// Try to load the requested file.
    if (fs::exists(filename))
    {
        CLOG(INFO, "StringDictionary") << "Loading " << filename;

        /// @todo FINISH ME
        return true;
    }
    else
    {
        CLOG(WARNING, "StringDictionary") << filename << " is missing; translations not loaded";

        return false;
    }
}

bool StringDictionary::add(std::string id_, std::string str_, std::string language_ = "")
{
    if (language_ == "")
    {
        language_ = language;
    }

    if (language_ != language)
    {
        return false;
    }

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

std::string const& StringDictionary::operator[](std::string id_) const
{
    return get(id_);
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
