#pragma once

#include <iostream>

/// Base class for objects that can be printed to STDOUT (e.g. for debugging purposes).
/// Adapted from http://0xfede.io/2015/12/13/T-C++-ObserverPattern.html
class Printable
{

public:
  virtual ~Printable() = default;

protected:
  friend std::ostream& operator<<(std::ostream& o, const Printable& object);

  virtual void printToStream(std::ostream& o) const;
};
