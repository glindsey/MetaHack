#pragma once

#include <functional>
#include <memory>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "Event.h"
#include "Printable.h"

// Forward declarations
using EventSet = std::unordered_set<EventID>;
using ObserversSet = std::unordered_set<Object*>;
using EventObservers = std::unordered_map<EventID, ObserversSet>;
using EventObserversPair = std::pair<EventID, ObserversSet>;
using EventQueue = std::queue<std::function<bool()>>;

/// An object which can broadcast and/or listen to events.
/// Observer pattern code adapted from http://0xfede.io/2015/12/13/T-C++-ObserverPattern.html
class Object : public Printable
{

public:
  struct Registration : public ConcreteEvent<Registration>
  {
    enum class State { Registered, Unregistered } state;
    void printToStream(std::ostream& o) const;
  };

  Object(EventSet const events);
  Object(EventSet const events, std::string name);
  virtual ~Object();

  std::string const& getName();

  /// Subscribes to an event emitted by an object.
  /// @param subject    Reference to object to subscribe to.
  /// @param eventID    ID of event to subscribe to, or EventID::All to
  ///                   subscribe to all events from this object.
  inline void subscribeTo(Object& subject, EventID eventID = EventID::All)
  {
    subject.addObserver(*this, eventID);
  }

  /// Subscribes to an event emitted by an object.
  /// @param subject    Pointer to object to subscribe to.
  /// @param eventID    ID of event to subscribe to, or EventID::All to
  ///                   subscribe to all events from this object.
  inline void subscribeTo(Object* subject, EventID eventID = EventID::All)
  {
    if (!subject) return;
    subject->addObserver(*this, eventID);
  }

  /// Subscribes to an event emitted by an object.
  /// @param subject    Ref to unique_ptr to object to subscribe to.
  /// @param eventID    ID of event to subscribe to, or EventID::All to
  ///                   subscribe to all events from this object.
  inline void subscribeTo(std::unique_ptr<Object>& subject, EventID eventID = EventID::All)
  {
    auto ptr = subject.get();
    if (!ptr) return;
    subject->addObserver(*this, eventID);
  }

  /// Unsubscribes from an event emitted by an object.
  /// @param subject    Reference to object to unsubscribe from.
  /// @param eventID    ID of event to unsubscribe to, from EventID::All to
  ///                   unsubscribe from all events from this object.
  inline void unsubscribeFrom(Object& subject, EventID eventID = EventID::All)
  {
    subject.removeObserver(*this, eventID);
  }

  /// Unsubscribes from an event emitted by an object.
  /// @param subject    Pointer to object to unsubscribe from.
  /// @param eventID    ID of event to unsubscribe to, from EventID::All to
  ///                   unsubscribe from all events from this object.
  inline void unsubscribeFrom(Object* subject, EventID eventID = EventID::All)
  {
    if (!subject) return;
    subject->removeObserver(*this, eventID);
  }

  /// Unsubscribes from an event emitted by an object.
  /// @param subject    Ref to unique_ptr to object to unsubscribe from.
  /// @param eventID    ID of event to unsubscribe to, from EventID::All to
  ///                   unsubscribe from all events from this object.
  inline void unsubscribeFrom(std::unique_ptr<Object>& subject, EventID eventID = EventID::All)
  {
    auto ptr = subject.get();
    if (!ptr) return;
    ptr->removeObserver(*this, eventID);
  }

  /// Adds an observer of an event emitted by this object.
  /// @param observer   Reference to observing object to add.
  /// @param eventID    ID of event to observe, or EventID::All to
  ///                   subscribe to all events.
  void addObserver(Object& observer, EventID eventID);

  /// Remove all observers of this object.
  /// Typically used before the object is destroyed.
  void removeAllObservers();

  /// Removes an observer of an event emitted by this object.
  /// @param observer   Reference to observer to remove.
  /// @param eventID    ID of event to remove from, or EventID::All to
  ///                   unsubscribe from all events.
  void removeObserver(Object& observer, EventID eventID = EventID::All);

  virtual void printToStream(std::ostream& o) const override;

protected:
  bool onEvent_NV(Event const& event);

  /// Virtual method call to handle events.
  /// Returns true if the event was handled and should STOP being passed
  /// along the chain, false otherwise.
  /// @note If the event was handled BUT you still want it to keep being
  ///       broadcast, return false!
  virtual bool onEvent(Event const& event);

  using BroadcastDelegate = std::function<bool(Event& event, bool shouldSend)>;
  using UnicastDelegate = std::function<void(Event& event, Object& observer, bool shouldSend)>;

  bool broadcast(Event& event);
  void unicast(Event& event, Object& observer);

  ObserversSet& getObservers(EventID eventID);
  ObserversSet const& getObservers(EventID eventID) const;
  bool observerIsObservingEvent(Object& observer, EventID eventID) const;

  virtual bool broadcast_(Event& event, BroadcastDelegate do_broadcast);
  virtual void unicast_(Event& event, Object& observer, UnicastDelegate do_unicast);

protected:

private:
  /// Optional name of this object. Aids in debugging.
  std::string m_name;

  /// Queue of onEvent calls to be made during a `broadcast()` call.
  EventQueue m_eventQueue;

  /// Observers of this object's different events.
  EventObservers m_eventObservers;

  /// Objects being observed by this object.
  std::unordered_map<Object*, int> m_observations;

};
