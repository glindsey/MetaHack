#pragma once

#include <utility>

#include "Event.h"

// Forward declarations
class Observable;

/// Class that implements an Observer of one or more objects.
/// Each object being observed must obey the following rules:
///   1. It must derive from `Observable`.
///   2. It must not be destroyed without notifying the Observer.
///      (If it is derived from `Observable` this will happen
///      automatically upon destruction.)
class Observer
{
public:
  /// Default constructor.
  Observer();

  /// Constructor that starts off observing an object.
  /// Registers with the object given.
  /// @param object   The object to observe.
  Observer(Observable& object);

  /// Destructor.
  /// Deregisters with observed object, if there is one.
  virtual ~Observer();

  /// Start observing an object.
  virtual void startObserving(Observable& observed);

  /// Stop observing an object.
  virtual void stopObserving(Observable& observed);

  /// Clear all observed objects.
  virtual void stopObservingAll();

  /// Final method for special event handling (such as "Destroyed").
  virtual void notifyOfEvent(Observable& observed, Event event) final;

protected:
  /// Get a const pointer to the vector of observed object.
  virtual std::vector<Observable*> const getObservedObjects() const final;

  /// Virtual method for event handling, overridden by subclasses.
  virtual void notifyOfEvent_(Observable& observed, Event event) = 0;

private:
  /// Vector of objects being observed.
  std::vector<Observable*> observedObjects;
};