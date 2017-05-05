/// @file Subject.cpp Subject implementation for observer pattern.
/// Adapted from http://0xfede.io/2015/12/13/T-C++-ObserverPattern.html

#include "stdafx.h"

#include "Subject.h"

#include <queue>
#include <unordered_set>

#include "AssertHelper.h"
#include "Event.h"
#include "Observer.h"

Subject::Subject()
  :
  m_eventsAlreadyRegistered{ false },
  m_eventQueue{},
  m_eventObservers{}
{}

Subject::~Subject()
{
  for (auto& prioritizedObservers : m_eventObservers)
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
  registerEventsIfNeeded(*this);

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
      << " for EventID " << eventID 
      << " with priority " << priority;

    observer.onEvent(e);
  }
}

void Subject::removeAllObservers()
{
  registerEventsIfNeeded(*this);

  Registration e;
  e.state = Registration::State::Unregistered;
  e.subject = this;

  for (auto& eventObservers : m_eventObservers)
  {
    auto& prioritizedObservers = eventObservers.second;

    for (auto& pair : prioritizedObservers)
    {
      auto& observersSet = pair.second;

      for (auto& observer : observersSet)
      {
        observer->onEvent(e);
      }
    }

    prioritizedObservers.clear();
  }

  CLOG(TRACE, "ObserverPattern") << "Deregistered all Observers for all Events";
}

void Subject::removeObserver(Observer& observer, EventID eventID)
{
  registerEventsIfNeeded(*this);

  size_t removalCount = 0;
  if (eventID == EventID::All)
  {
    for (auto& eventObservers : m_eventObservers)
    {
      auto& prioritizedObservers = eventObservers.second;

      for (auto& pair : prioritizedObservers)
      {
        auto& observersSet = pair.second;
        removalCount += observersSet.erase(&observer);
      }
    }

    CLOG(TRACE, "ObserverPattern") << "Deregistered Observer " << observer << " for all Events";
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

  if (removalCount == 0)
  {
    CLOG(WARNING, "ObserverPattern") 
      << "Observer " << observer 
      << " was not registered for " 
      << ((eventID == EventID::All) ? "any Events" : ("Event " + eventID))
      << " from Subject " << *this;
  }

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

bool Subject::broadcast(Event& event)
{
  bool result = true;

  registerEventsIfNeeded(*this);

  event.subject = this;

  return broadcast_(event, [&](Event& event, bool shouldSend) -> bool
  {
    if (shouldSend)
    {
      auto finalEvent = event.heapClone();

      auto dispatchBlock = [=]() mutable -> bool
      {
        auto observerCount = getObserverCount(finalEvent->getId());

        // Return if no observers for this event.
        if (observerCount == 0) return true;

        auto& prioritizedObservers = getObservers(finalEvent->getId());

        CLOG(TRACE, "ObserverPattern") << "Broadcasting: " << *finalEvent;

        bool keep_going = true;
        for (auto& priorityPair : prioritizedObservers)
        {
          for (auto& observer : priorityPair.second)
          {
            keep_going = observer->onEvent(*finalEvent);
            if (!keep_going) break;
          }
          if (!keep_going) break;
        }

        if (!keep_going)
        {
          CLOG(TRACE, "ObserverPattern") << "Broadcast Halted: " << *finalEvent;
        }
        delete finalEvent;

        m_eventQueue.pop();
        if (!m_eventQueue.empty())
        {
          m_eventQueue.front()();
        }

        return keep_going;
      };

      m_eventQueue.push(dispatchBlock);
      if (m_eventQueue.size() == 1)
      {
        result = m_eventQueue.front()();
      }
    }

    return result;
  });
}

void Subject::unicast(Event& event, Observer& observer)
{
  registerEventsIfNeeded(*this);

  event.subject = this;

  unicast_(event, observer, [&](Event& event, Observer& observer, bool shouldSend)
  {
    if (shouldSend)
    {
      auto finalEvent = event.heapClone();

      auto dispatchBlock = [=, &observer]() mutable -> bool
      {
        bool result = true;

        if (observerIsObservingEvent(observer, finalEvent->getId()))
        {
          CLOG(TRACE, "ObserverPattern") << "Unicasting " << *finalEvent << " to " << observer;
          result = observer.onEvent(*finalEvent);
        }

        delete finalEvent;

        m_eventQueue.pop();
        if (!m_eventQueue.empty())
        {
          m_eventQueue.front()();
        }

        return result;
      };

      m_eventQueue.push(dispatchBlock);
      if (m_eventQueue.size() == 1)
      {
        m_eventQueue.front()();
      }
    }
  });
}

size_t Subject::getObserverCount(EventID eventID) const
{
  size_t count = 0;

  auto& eventObserverIter = m_eventObservers.find(eventID);

  if (eventObserverIter != m_eventObservers.end())
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
  auto& eventObserversIter = m_eventObservers.find(eventID);

  Assert("ObserverPattern", (eventObserversIter != m_eventObservers.end()),
         "\nReason:\tattempted to get observers for an unregistered event." <<
         "\nSubject:\t" << *this <<
         "\nEventID:\t" << eventID);

  return eventObserversIter->second;
}

PrioritizedObservers const& Subject::getObservers(EventID eventID) const
{
  auto const& eventObserversIter = m_eventObservers.find(eventID);

  Assert("ObserverPattern", (eventObserversIter != m_eventObservers.end()),
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

bool Subject::broadcast_(Event& event, BroadcastDelegate do_broadcast)
{
  return do_broadcast(event, true);
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

void Subject::registerEventsIfNeeded(Subject const& subject)
{
  if (!m_eventsAlreadyRegistered)
  {
    m_eventsAlreadyRegistered = true;
    std::unordered_set<EventID> events = subject.registeredEvents();
    for (EventID const registeredEvent : events)
    {
      m_eventObservers.emplace(registeredEvent, PrioritizedObservers());
    }
  }
}
