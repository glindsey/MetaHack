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
  bool onEvent(Event const& event);

  /// Virtual method call to handle events.
  /// Returns true if the event was handled and should STOP being passed
  /// along the chain, false otherwise.
  /// @note If the event was handled BUT you still want it to keep being
  ///       broadcast, return false!

  virtual bool onEvent_V(Event const& event) = 0;

private:
  std::unordered_map<Subject*, int> m_observations;
};