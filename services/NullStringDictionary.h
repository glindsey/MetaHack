#pragma once

#include "services/IStringDictionary.h"

class NullStringDictionary : public IStringDictionary
{
public:
  /// Create a null string dictionary.
  NullStringDictionary() {}

  virtual bool loadFile(std::string filename_) override
  {
    return true;
  }

  virtual bool add(std::string id_, std::string str_) override
  {
    return true;
  }

  virtual std::string get(std::string id_) const override
  {
    return id_;
  }

  /// Clears out the dictionary.
  virtual void clear() override {}
};