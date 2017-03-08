#pragma once

#include <utility>

#include "Serializable.h"

// Forward declarations
class Event;

/// Observer declaration for observer pattern.
/// Adapted from http://0xfede.io/2015/12/13/T-C++-ObserverPattern.html
class Observer : public Serializable
{
  friend class Subject;

public:
  Observer();
  virtual ~Observer() = 0;

protected:
  virtual void onEvent(Event const& event);

private:
  class Impl;
  const std::unique_ptr<Impl> pImpl;
};