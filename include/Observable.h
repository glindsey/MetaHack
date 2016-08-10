#pragma once

#include <vector>

#include "Event.h"
#include "Observer.h"

/// Template class that implements an object that is observable by an Observer.
/// A class `T` derived from this template should derive from `Observable<T>`.
template <class SubclassType>
class Observable
{
public:
  /// Destructor.
  /// Notifies all observers of this object that it is being deleted.
  ~Observable()
  {
    for (auto observer : observers)
    {
      Event event;
      observer->notifyOfEvent(event);
    }
  }

  /// Register an observer with this observable.
  void registerObserver(Observer<SubclassType>& observer)
  {
    observers.push_back(observer);
  }

  /// Deregister an observer with this observable.
  void deregisterObserver(Observer<SubclassType>& observer)
  {
    auto& foundObserver = std::find(observers.begin(), observers.end(), &observer);
    if (foundObserver != observers.end())
    {
      observers.erase(foundObserver);
    }
  }

protected:

private:
  /// Vector of observers of this object.
  std::vector<Observer<SubclassType>*> observers;
};