#pragma once

#include <boost/noncopyable.hpp>

#include <string>

#include "json.hpp"
using json = ::nlohmann::json;

namespace Config
{
  /// Extending the metaphors I've used previously, this object contains the
  /// "game bible", e.g. the definitions for game entities and concepts.
  class Bible : public boost::noncopyable
  {
  public:
    virtual ~Bible();

    /// Get data for a specific Entity category.
    /// If it doesn't exist, attempt to load it.
    json& categoryData(std::string name);

    /// Get reference to game rules data.
    inline json& data()
    {
      return m_data;
    }

    /// Get const reference to game rules data.
    inline json const& data() const
    {
      return m_data;
    }

    /// Get instance reference.
    friend Bible& bible();

  protected:
    Bible();

    /// Attempt to load JSON data for an entity category.
    /// Also runs any associated Lua script.
    void loadCategoryIfNecessary(std::string name);

    void loadTemplateComponentsFor(std::string name);

  private:
    /// Game rules data, as stored in a JSON object.
    json m_data;
  };

  Bible& bible();

} // end namespace Config
