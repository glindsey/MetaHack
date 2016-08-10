#pragma once

#include <utility>

#include "Event.h"


/// Template class that implements an Observer of a particular object.
/// The object `Obj` being observed must obey the following rules:
///   1. It must derive from `Observable<Obj>`.
///   2. It must not be destroyed without notifying the Observer.
///      (If it is derived from `Observable<Obj>` this will happen
///      automatically upon destruction.)
template <class ObservedType>
class Observer
{
public:
  /// Default constructor.
  Observer()
    :
    observedObject{ nullptr }
  {}

  /// Constructor that starts off observing an object.
  /// Registers with the object given.
  /// @param object   The object to observe.
  Observer(ObservedType& object)
  {
    setObservedObject(object);
  }

  /// Destructor.
  /// Deregisters with observed object, if there is one.
  virtual ~Observer()
  {
    clearObservedObjectIfNecessary();
  }

  /// Set the object to be observed.
  /// If an object is already being observed, this method will first
  /// deregister with that object.
  void setObservedObject(ObservedType& object)
  {
    clearObservedObjectIfNecessary();
    object.registerObserver(*this);
    observedObject = &object;
  }

  /// Clear the observed object, if necessary.
  /// If no object is being observed, this method simply returns.
  void clearObservedObjectIfNecessary()
  {
    if (observedObject != nullptr)
    {
      observedObject->deregisterObserver(*this);
      observedObject = nullptr;
    }
  }

  /// Concrete method for special event handling (such as "Destroyed").
  void notifyOfEvent(Event& event)
  {
    notifyOfEvent_(event);
    if (event == Event::Destroyed)
    {
      observedObject = nullptr;
    }
  }


protected:
  /// Get a const pointer to the observed object.
  ObservedType const* getObservedObject()
  {
    return observedObject;
  }

  /// Template virtual method for event handling, overridden by subclasses.
  virtual void notifyOfEvent_(Event& event)
  {

  }

private:
  /// Pointer to object being observed.
  /// Equal to nullptr if no object is currently being observed.
  ObservedType*  observedObject;
};