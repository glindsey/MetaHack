#pragma once

#include "services/IStrings.h"

class NullStrings : public IStrings
{
public:
  /// Create a null string dictionary.
  NullStrings() {}

  virtual bool loadFile(std::string filename_) override
  {
    return true;
  }

  virtual bool add(std::string id_, std::string str_) override
  {
    return true;
  }

  virtual bool contains(std::string id_) const override
  {
    return false;
  }

  virtual std::string get(std::string id_) const override
  {
    return id_;
  }

  /// Clears out the dictionary.
  virtual void clear() override {}
};