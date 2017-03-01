#include "stdafx.h"

#include "Observable.h"
#include "Observer.h"

Observable::~Observable()
{
  notifyObservers(Event::Destroyed);
}

void Observable::registerObserver(Observer& observer)
{
  auto& foundObserver = std::find(observers.begin(), observers.end(), &observer);
  if (foundObserver == observers.end())
  {
    observers.push_back(&observer);
    //observer.notifyOfEvent(*this, Event::Registered); // <-- can NOT do this safely because registration is usually done in constructors
  }
}

void Observable::deregisterObserver(Observer& observer)
{
  auto& foundObserver = std::find(observers.begin(), observers.end(), &observer);
  if (foundObserver != observers.end())
  {
    observers.erase(foundObserver);
  }
}

void Observable::notifyObservers(Event event)
{
  for (auto observer : observers)
  {
    observer->notifyOfEvent(*this, event);
  }
}