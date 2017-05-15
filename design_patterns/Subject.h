#pragma once

#include <functional>
#include <memory>
#include <unordered_set>

#include "Event.h"
#include "Serializable.h"

// Forward declarations
class Observer;
using ObserversSet = std::unordered_set<Observer*>;
using EventObservers = std::unordered_map<EventID, ObserversSet>;
using EventObserversPair = std::pair<EventID, ObserversSet>;
using EventQueue = std::queue<std::function<bool()>>;

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

  Subject(std::unordered_set<EventID> const events);
  virtual ~Subject();

  /// Adds an observer of an event emitted by this subject.
  /// @param observer   Reference to observer to add.
  /// @param eventID    ID of event to observe, or EventID::All to
  ///                   subscribe to all events.
  void addObserver(Observer& observer, EventID eventID);

  /// Remove all observers of this subject.
  /// Typically used before the subject is destroyed.
  void removeAllObservers();

  /// Removes an observer of an event emitted by this subject.
  /// @param observer   Reference to observer to remove.
  /// @param eventID    ID of event to remove from, or EventID::All to
  ///                   unsubscribe from all events.
  void removeObserver(Observer& observer, EventID eventID = EventID::All);

protected:
  using BroadcastDelegate = std::function<bool(Event& event, bool shouldSend)>;
  using UnicastDelegate = std::function<void(Event& event, Observer& observer, bool shouldSend)>;

  bool broadcast(Event& event);
  void unicast(Event & event, Observer & observer);

  ObserversSet& getObservers(EventID eventID);
  ObserversSet const& getObservers(EventID eventID) const;
  bool Subject::observerIsObservingEvent(Observer& observer, EventID eventID) const;

  virtual bool broadcast_(Event& event, BroadcastDelegate do_broadcast);
  virtual void unicast_(Event& event, Observer& observer, UnicastDelegate do_unicast);

protected:

private:
  EventQueue m_eventQueue;
  EventObservers m_eventObservers;
};