#pragma once

#include <string>

/// Interface for obtaining pathnames.
class IPaths
{
public:
  virtual ~IPaths() = 0;
  
  /// Return a default object that implements this interface.
  static IPaths* getDefault();
  
  /// Get const reference to the resources path.
  virtual std::string const& resources() = 0;
  
  /// Get const reference to the log path.
  virtual std::string const& logs() = 0;
};
