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
  using BroadcastDelegate = std::function<void(Event& event, bool shouldSend)>;
  using UnicastDelegate = std::function<void(Event& event, Observer& observer, bool shouldSend)>;

  void broadcast(Event& event);
  void unicast(Event & event, Observer & observer);

  virtual void broadcast_(Event& event, BroadcastDelegate do_broadcast);
  virtual void unicast_(Event& event, Observer& observer, UnicastDelegate do_unicast);

private:
  class Impl;
  const std::unique_ptr<Impl> pImpl;
};