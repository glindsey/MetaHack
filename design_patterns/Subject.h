#pragma once

#include <functional>
#include <memory>
#include <unordered_set>

#include "Event.h"
#include "Serializable.h"

// Forward declarations
class Observer;

/// Subject declaration for observer pattern.
/// Adapted from http://0xfede.io/2015/12/13/T-C++-ObserverPattern.html
class Subject : public virtual Serializable
{

public:
  struct Registration : public ConcreteEvent<Registration>
  {
    enum class State { Registered, Unregistered } state;
    void serialize(std::ostream& o) const;
  };

  Subject();
  virtual ~Subject();

  void addObserver(Observer& observer, EventID eventID);
  void removeObserver(Observer& observer, EventID eventID = EventID::All);

  /// Return the set of events this Subject provides.
  /// By default, returns nothing. Must be overridden with the events
  /// declared in the Subject subclass.
  using RegisteredEventsDelegate = std::function<std::unordered_set<EventID>()>;
  virtual std::unordered_set<EventID> registeredEvents() const;

protected:
  void broadcast(Event& event);

  using BroadcastDelegate = std::function<void(Event& event, bool shouldBroadcast)>;
  virtual void broadcast_(Event& event, BroadcastDelegate broadcast_delegate);

private:
  class Impl;
  const std::unique_ptr<Impl> pImpl;
};