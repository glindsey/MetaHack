#pragma once

#include <string>

#include "json.hpp"
using json = ::nlohmann::json;

/// Interface for game rules.
class IGameRules
{
public:
  virtual ~IGameRules() = 0;

  /// Return a Null Object that implements this interface.
  static IGameRules* getDefault();

  /// Get data for a specific Entity category.
  /// If it doesn't exist, attempt to load it.
  virtual json& categoryData(std::string name) = 0;

  /// Get reference to game rules data.
  virtual json& data() = 0;

  /// Get const reference to game rules data.
  virtual json const& data() const = 0;  
};