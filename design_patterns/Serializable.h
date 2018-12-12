#pragma once

#include <iostream>

/// Base class for objects that can be serialized.
/// Adapted from http://0xfede.io/2015/12/13/T-C++-ObserverPattern.html
class Serializable
{

public:
  virtual ~Serializable() = default;

protected:
  friend std::ostream& operator<<(std::ostream& o, const Serializable& object);

  virtual void serialize(std::ostream& o) const;
};
