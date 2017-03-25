/// @file Subject.cpp Subject implementation for observer pattern.
/// Adapted from http://0xfede.io/2015/12/13/T-C++-ObserverPattern.html

#include "stdafx.h"

#include "Subject.h"

#include <queue>
#include <unordered_set>

#include "AssertHelper.h"
#include "Event.h"
#include "Observer.h"

class Subject::Impl
{

public:
  bool eventsAlreadyRegistered;
  EventQueue eventQueue;
  EventObservers eventObservers;
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
      eventObservers.emplace(registeredEvent, PrioritizedObservers());
    }
  }
}

Subject::Subject() :
  pImpl(new Impl())
{}

Subject::~Subject()
{
  for (auto& prioritizedObservers : pImpl->eventObservers)
  {
    auto eventID = prioritizedObservers.first;
    auto observerCount = getObserverCount(eventID);

    Assert("ObserverPattern", (observerCount == 0),
            "\nReason:\tsubject went out of scope while at least one observer is still registered for one of its events" <<
            "\nSubject:\t" << this <<
            "\nEvent:\t" << eventID);
  }
}

void Subject::addObserver(Observer& observer, EventID eventID, ObserverPriority priority)
{
  pImpl->registerEventsIfNeeded(*this);

  if (eventID == EventID::All)
  {
    for (auto& id : registeredEvents())
    {
      addObserver(observer, id, priority);
    }
  }
  else
  {
    Assert("ObserverPattern", !observerIsObservingEvent(observer, eventID),
           "\nReason:\tattempted to add an observer for an event it is already observing." <<
           "\nSubject:\t" << *this <<
           "\nObserver:\t" << observer <<
           "\nEvent:\t" << eventID);

    auto& prioritizedObservers = getObservers(eventID);
    auto insertResult = prioritizedObservers.insert({ priority, ObserversSet() });
    auto& observersSet = (insertResult.first)->second;
    observersSet.insert(&observer);

    Registration e;
    e.state = Registration::State::Registered;
    e.subject = this;

    CLOG(TRACE, "ObserverPattern") << "Registered Observer " << observer 
      << "for EventID " << eventID 
      << " with priority " << priority;

    observer.onEvent(e);
  }
}

void Subject::removeObserver(Observer& observer, EventID eventID)
{
  pImpl->registerEventsIfNeeded(*this);

  size_t removalCount = 0;
  if (eventID == EventID::All)
  {
    for (auto& eventObservers : pImpl->eventObservers)
    {
      auto& prioritizedObservers = eventObservers.second;

      for (auto& pair : prioritizedObservers)
      {
        auto& observersSet = pair.second;
        removalCount += observersSet.erase(&observer);
      }
    }

    CLOG(TRACE, "ObserverPattern") << "Deregistered Observer " << observer << "for all Events";
  }
  else
  {
    auto observerCount = getObserverCount(eventID);

    auto& prioritizedObservers = getObservers(eventID);

    Assert("ObserverPattern", (observerCount > 0),
           "\nReason:\tattempted to remove an observer on event with no observers." <<
           "\nSubject:\t" << *this <<
           "\nObserver:\t" << observer <<
           "\nEvent:\t" << eventID);

    for (auto& pair : prioritizedObservers)
    {
      auto& observersSet = pair.second;
      removalCount += observersSet.erase(&observer);
    }

    CLOG(TRACE, "ObserverPattern") << "Deregistered Observer " << observer 
      << "for EventID " << eventID;
  }

  Assert("ObserverPattern", removalCount != 0,
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

  broadcast_(event, [&](Event& event, bool shouldSend)
  {
    if (shouldSend)
    {
      auto finalEvent = event.heapClone();

      auto dispatchBlock = [=]() mutable
      {
        auto observerCount = getObserverCount(finalEvent->getId());
        auto& prioritizedObservers = getObservers(finalEvent->getId());

        CLOG(TRACE, "ObserverPattern") << "Broadcasting: " << *finalEvent;

        // Return if no observers for this event.
        if (observerCount == 0) return;

        bool keepGoing = true;
        for (auto& priorityPair : prioritizedObservers)
        {
          for (auto& observer : priorityPair.second)
          {
            keepGoing = observer->onEvent(*finalEvent);
            if (!keepGoing) break;
          }
          if (!keepGoing) break;
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

void Subject::unicast(Event& event, Observer& observer)
{
  pImpl->registerEventsIfNeeded(*this);

  event.subject = this;

  unicast_(event, observer, [&](Event& event, Observer& observer, bool shouldSend)
  {
    if (shouldSend)
    {
      auto finalEvent = event.heapClone();

      auto dispatchBlock = [=, &observer]() mutable
      {
        Assert("ObserverPattern", observerIsObservingEvent(observer, finalEvent->getId()),
               "\nReason:\tattempted to notify observer not registered for event." <<
               "\nSubject:\t" << *this <<
               "\nObserver:\t" << observer <<
               "\nEvent:\t" << *finalEvent);

        CLOG(TRACE, "ObserverPattern") << "Unicasting " << *finalEvent << " to " << observer;

        observer.onEvent(*finalEvent);

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

size_t Subject::getObserverCount(EventID eventID) const
{
  size_t count = 0;

  auto& eventObserverIter = pImpl->eventObservers.find(eventID);

  if (eventObserverIter != pImpl->eventObservers.end())
  {
    auto& prioritizedObservers = eventObserverIter->second;
    for (auto& observers : prioritizedObservers)
    {
      count += observers.second.size();
    }
    return count;
  }
  return 0;
}

PrioritizedObservers& Subject::getObservers(EventID eventID)
{
  auto& eventObserversIter = pImpl->eventObservers.find(eventID);

  Assert("ObserverPattern", (eventObserversIter != pImpl->eventObservers.end()),
         "\nReason:\tattempted to get observers for an unregistered event." <<
         "\nSubject:\t" << *this <<
         "\nEventID:\t" << eventID);

  return eventObserversIter->second;
}

PrioritizedObservers const& Subject::getObservers(EventID eventID) const
{
  auto const& eventObserversIter = pImpl->eventObservers.find(eventID);

  Assert("ObserverPattern", (eventObserversIter != pImpl->eventObservers.end()),
         "\nReason:\tattempted to get observers for an unregistered event." <<
         "\nSubject:\t" << *this <<
         "\nEventID:\t" << eventID);

  return eventObserversIter->second;
}

bool Subject::observerIsObservingEvent(Observer& observer, EventID eventID) const
{
  auto& prioritizedObservers = getObservers(eventID);

  for (auto& prioritiesPair : prioritizedObservers)
  {
    if (prioritiesPair.second.count(&observer) != 0) return true;
  }

  return false;
}

void Subject::broadcast_(Event& event, BroadcastDelegate do_broadcast)
{
  do_broadcast(event, true);
}

void Subject::unicast_(Event& event, Observer& observer, UnicastDelegate do_unicast)
{
  do_unicast(event, observer, true);
}

void Subject::Registration::serialize(std::ostream& o) const
{
  Event::serialize(o);
  o << " | registration state: " <<
    (state == State::Registered ? "Registered" : "Unregistered");
}