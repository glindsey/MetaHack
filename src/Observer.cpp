#include "stdafx.h"

#include "Observer.h"
#include "Observable.h"

Observer::Observer()
{}

Observer::Observer(Observable& object)
{
  startObserving(object);
}

Observer::~Observer()
{
  stopObservingAll();
}

void Observer::startObserving(Observable& observed)
{
  auto& foundObserved = std::find(observedObjects.begin(), observedObjects.end(), &observed);
  if (foundObserved == observedObjects.end())
  {
    observed.registerObserver(*this);
    observedObjects.push_back(&observed);
  }
}

void Observer::stopObserving(Observable& observed)
{
  auto& foundObserved = std::find(observedObjects.begin(), observedObjects.end(), &observed);
  if (foundObserved != observedObjects.end())
  {
    observed.deregisterObserver(*this);
    observedObjects.erase(foundObserved);
  }
}

void Observer::stopObservingAll()
{
  for (auto& observed : observedObjects)
  {
    observed->deregisterObserver(*this);
  }
  observedObjects.clear();
}

void Observer::notifyOfEvent(Observable& observed, Event& event)
{
  notifyOfEvent_(observed, event);
  if (event == Event::Destroyed)
  {
    // Can't call stopObserving() as we don't want to call deregisterObserver().
    auto& foundObserved = std::find(observedObjects.begin(), observedObjects.end(), &observed);
    if (foundObserved != observedObjects.end())
    {
      observedObjects.erase(foundObserved);
    }
  }
}

/// Get a const pointer to the observed object.
std::vector<Observable*> const Observer::getObservedObjects() const
{
  return observedObjects;
}