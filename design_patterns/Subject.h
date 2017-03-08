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

protected:
  /// Return the set of events this Subject provides.
  /// By default, returns nothing. Most be overridden with the events
  /// declared in the Subject subclass.
  virtual std::unordered_set<EventID> registeredEvents() const;

  void broadcast(Event& event);
  using BroadcastDelegate = std::function<void(Event& event, bool shouldBroadcast)>;
  virtual void shouldBroadcast(Event& event, BroadcastDelegate shouldBroadcast);

private:
  class Impl;
  const std::unique_ptr<Impl> pImpl;
};