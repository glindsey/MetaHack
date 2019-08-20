#pragma once

#include <iostream>

/// Base class for objects that can be printed to STDOUT (e.g. for debugging purposes).
/// Adapted from Serializable in http://0xfede.io/2015/12/13/T-C++-ObserverPattern.html
class Printable
{

public:
  virtual ~Printable() = default;

  friend std::ostream& operator<<(std::ostream& o, const Printable& object);

  /// By default this returns the class name and the address of the object.
  /// It can, of course, be overridden by child classes.
  virtual std::string toString() const;

  virtual void printToStream(std::ostream& o) const;
};
