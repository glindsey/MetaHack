#pragma once

#include <utility>

#include "Serializable.h"

// Forward declarations
class Event;

/// Enum classes for EventResult struct.
enum class EventHandled { No, Yes };
enum class ContinueBroadcasting { No, Yes };

/// Return status struct for onEvent_NVI().
struct EventResult
{
  EventHandled event_handled;
  ContinueBroadcasting continue_broadcasting;
};

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

  virtual EventResult onEvent_NVI(Event const& event) = 0;

private:
  class Impl;
  const std::unique_ptr<Impl> pImpl;
};