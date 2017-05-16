#pragma once

#include "services/IGameRules.h"

#include "json.hpp"
using json = ::nlohmann::json;

/// Implementation for a null game rules service.
class NullGameRules : public IGameRules
{
public:
  virtual ~NullGameRules() {}

  /// Get data for a specific Entity category.
  /// If it doesn't exist, attempt to load it.
  virtual json& category(std::string name, std::string subType = "")
  {
    std::string fullName = subType + "." + name;
    if (m_data.count(fullName) == 0)
    {
      m_data[fullName] = json::object();
    }
    return m_data[fullName];
  }

  /// Get reference to game rules data.
  virtual json& data()
  {
    return m_data;
  }

  /// Get const reference to game rules data.
  virtual json const& data() const
  {
    return m_data;
  }

private:
  json m_data;
};