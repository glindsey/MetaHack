#pragma once

#include <vector>

#include "Event.h"

// Forward declarations
class Observer;

/// Class that implements an object that is observable by an Observer.
class Observable
{
  friend class Observer;

public:
  /// Destructor.
  /// Notifies all observers of this object that it is being deleted.
  virtual ~Observable();

  /// Notify all observers of an event.
  virtual void notifyObservers(Event event);

protected:
  /// Register an observer with this observable.
  virtual void registerObserver(Observer& observer);

  /// Deregister an observer with this observable.
  virtual void deregisterObserver(Observer& observer);

private:
  /// Vector of observers of this object.
  std::vector<Observer*> observers;
};