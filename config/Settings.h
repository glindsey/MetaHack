#pragma once

#include <boost/noncopyable.hpp>

#include "properties/PropertyDictionary.h"

namespace Config
{

  class Settings : public PropertyDictionary, public boost::noncopyable
  {
  public:
    virtual ~Settings();

    /// Get instance reference.
    friend Settings& settings();

  protected:
    Settings();

  private:

  };

  Settings& settings();

} // end namespace Config
