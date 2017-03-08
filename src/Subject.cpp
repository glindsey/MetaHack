#include "stdafx.h"

#include "Subject.h"

#include <queue>
#include <unordered_set>

#include "Assert.h"
#include "Event.h"
#include "Observer.h"

/// Subject implementation for observer pattern.
/// Adapted from http://0xfede.io/2015/12/13/T-C++-ObserverPattern.html
class Subject::Impl
{

public:
  bool eventsAlreadyRegistered;
  std::queue<std::function<void()>> eventQueue;
  std::unordered_map<EventID, std::unordered_set<Observer*>> observers;
  void registerEventsIfNeeded(Subject const& subject);
};

void Subject::Impl::registerEventsIfNeeded(Subject const& subject)
{
  if (!eventsAlreadyRegistered)
  {
    eventsAlreadyRegistered = true;
    std::unordered_set<EventID> events = subject.registeredEvents();
    for (EventID const registeredEvent : events)
    {
      observers.emplace(registeredEvent, std::unordered_set<Observer*>());
    }
  }
}

Subject::Subject() :
  pImpl(new Impl())
{}

Subject::~Subject()
{
  for (auto& observers : pImpl->observers)
  {
    Assert(!observers.second.size(),
           "\nReason:\tsubject went out of scope while at least one observer is still registered for one of its events" <<
           "\nSubject:\t" << this <<
           "\nObserver:\t" << *observers.second.begin() <<
           "\nEvent:\t" << observers.first);
  }
}

void Subject::addObserver(Observer& observer, EventID eventID)
{
  pImpl->registerEventsIfNeeded(*this);

  auto observers = pImpl->observers.find(eventID);
  Assert(observers != pImpl->observers.end(),
         "\nReason:\tattempted to add an observer for an unregistered event." <<
         "\nSubject:\t" << *this <<
         "\nObserver:\t" << observer <<
         "\nEvent:\t" << eventID);

  bool observerIsNotAlreadyObserving = observers->second.insert(&observer).second;
  Assert(observerIsNotAlreadyObserving,
         "\nReason:\tattempted to add an observer for an event it is already observing." <<
         "\nSubject:\t" << *this <<
         "\nObserver:\t" << observer <<
         "\nEvent:\t" << eventID);

  Registration e;
  e.state = Registration::State::Registered;
  e.subject = this;

  observer.onEvent(e);
}

void Subject::removeObserver(Observer& observer, EventID eventID)
{
  pImpl->registerEventsIfNeeded(*this);

  size_t removalCount = 0;
  if (eventID == EventID::All)
  {
    for (auto& observers : pImpl->observers)
    {
      removalCount += observers.second.erase(&observer);
    }
  }
  else
  {
    auto observers = pImpl->observers.find(eventID);
    Assert(observers != pImpl->observers.end(),
           "\nReason:\tattempted to remove an observer for an unregistered event." <<
           "\nSubject:\t" << *this <<
           "\nObserver:\t" << observer <<
           "\nEvent:\t" << eventID);

    removalCount = observers->second.erase(&observer);
  }

  Assert(removalCount != 0,
         "\nReason:\tattempted to remove an observer not observing an event." <<
         "\nSubject:\t" << *this <<
         "\nObserver:\t" << observer <<
         "\nEvent:\t" << eventID);

  Registration e;
  e.state = Registration::State::Unregistered;
  e.subject = this;

  for (int i = 0; i < removalCount; i++)
  {
    observer.onEvent(e);
  }
}

std::unordered_set<EventID> Subject::registeredEvents() const
{
  return std::unordered_set<EventID>();
}

void Subject::broadcast(Event& event)
{
  pImpl->registerEventsIfNeeded(*this);

  event.subject = this;

  shouldBroadcast(event, [&](Event& event, bool shouldBroadcast)
  {
    if (shouldBroadcast)
    {

      auto finalEvent = event.heapClone();

      auto dispatchBlock = [=]() mutable
      {
        auto observers = pImpl->observers.find(finalEvent->getId());
        Assert(observers != pImpl->observers.end(),
               "\nReason:\tattempted to notify observers for an unregistered event." <<
               "\nSubject:\t" << *this <<
               "\nEvent:\t" << *finalEvent);

        for (Observer* observer : observers->second)
        {
          observer->onEvent(*finalEvent);
        }

        delete finalEvent;

        pImpl->eventQueue.pop();
        if (!pImpl->eventQueue.empty())
        {
          pImpl->eventQueue.front()();
        }
      };

      pImpl->eventQueue.push(dispatchBlock);
      if (pImpl->eventQueue.size() == 1)
      {
        pImpl->eventQueue.front()();
      }
    }
  });
}

void Subject::shouldBroadcast(Event &event, BroadcastDelegate shouldBroadcast)
{
  shouldBroadcast(event, true);
}

void Subject::Registration::serialize(std::ostream& o) const
{
  Event::serialize(o);
  o << " | registration state: " <<
    (state == State::Registered ? "Registered" : "Unregistered");
}