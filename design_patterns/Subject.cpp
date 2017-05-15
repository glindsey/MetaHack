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
  bool observersRemain = false;
  for (auto& prioritizedObservers : m_eventObservers)
  {
    auto eventID = prioritizedObservers.first;
    auto observersSet = getObservers(eventID);

    if (observersSet.size() != 0)
    {
      CLOG(WARNING, "ObserverPattern") << "Subject was deleted while " << observersSet.size() << " objects were still observing event " << eventID;
      observersRemain = true;
    }
  }

  if (observersRemain)
  {
    CLOG(WARNING, "ObserverPattern") << "Clearing all observers of this subject";
    removeAllObservers();
  }
}

void Subject::addObserver(Observer& observer, EventID eventID)
{
  registerEventsIfNeeded(*this);

  if (eventID == EventID::All)
  {
    for (auto& id : registeredEvents())
    {
      addObserver(observer, id);
    }
  }
  else
  {
    Assert("ObserverPattern", !observerIsObservingEvent(observer, eventID),
           "\nReason:\tattempted to add an observer for an event it is already observing." <<
           "\nSubject:\t" << *this <<
           "\nObserver:\t" << observer <<
           "\nEvent:\t" << eventID);

    auto& observersSet = getObservers(eventID);
    observersSet.insert(&observer);

    Registration e;
    e.state = Registration::State::Registered;
    e.subject = this;

    CLOG(TRACE, "ObserverPattern") << "Registered Observer " << observer 
      << " for EventID 0x" 
      << std::setbase(16) << std::setfill('0') << std::setw(8) << eventID;

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
    auto& observersSet = eventObservers.second;
    for (auto& observer : observersSet)
    {
      observer->onEvent(e);
    }

    observersSet.clear();
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
      auto& observersSet = eventObservers.second;
      removalCount += observersSet.erase(&observer);
    }

    CLOG(TRACE, "ObserverPattern") << "Deregistered Observer " << observer << " for all Events";
  }
  else
  {
    auto& observersSet = getObservers(eventID);

    Assert("ObserverPattern", (observersSet.size() > 0),
           "\nReason:\tattempted to remove an observer on event with no observers." <<
           "\nSubject:\t" << *this <<
           "\nObserver:\t" << observer <<
           "\nEvent:\t" << eventID);

    removalCount = observersSet.erase(&observer);

    CLOG(TRACE, "ObserverPattern") << "Deregistered Observer " << observer 
      << " for EventID 0x"
      << std::setbase(16) << std::setfill('0') << std::setw(8) << eventID;
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
      Event* copiedEvent = event.heapClone();

      auto dispatchBlock = [=]() mutable -> bool
      {
        bool handled = false;

        // Dispatch block now owns copiedEvent.
        CLOG(TRACE, "ObserverPattern") << "Broadcasting: " << *copiedEvent;

        auto id = copiedEvent->getId();
        auto& observersSet = getObservers(id);

        // Return if no observers for this event.
        if (observersSet.size() == 0)
        {
          CLOG(TRACE, "ObserverPattern") << "No observers registered for event";
          handled = true;
        }
        else
        {
          for (auto& observer : observersSet)
          {
            handled = observer->onEvent(*copiedEvent);
            if (handled) break;
          }

          if (handled)
          {
            CLOG(TRACE, "ObserverPattern") << "Broadcast Halted: " << *copiedEvent;
          }
        }

        delete copiedEvent;
        m_eventQueue.pop();
        if (!m_eventQueue.empty())
        {
          m_eventQueue.front()();
        }

        return handled;
      }; // end of dispatchBlock

      m_eventQueue.push(dispatchBlock);
      if (!m_eventQueue.empty())
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

ObserversSet& Subject::getObservers(EventID eventID)
{
  auto& eventObserversIter = m_eventObservers.find(eventID);

  Assert("ObserverPattern", (m_eventObservers.count(eventID) != 0),
         "\nReason:\tattempted to get observers for an unregistered event." <<
         "\nSubject:\t" << *this <<
         "\nEventID:\t0x" << 
         std::setbase(16) << std::setfill('0') << std::setw(8) << eventID);

  return m_eventObservers.at(eventID);
}

ObserversSet const& Subject::getObservers(EventID eventID) const
{
  auto& eventObserversIter = m_eventObservers.find(eventID);

  Assert("ObserverPattern", (m_eventObservers.count(eventID) != 0),
         "\nReason:\tattempted to get observers for an unregistered event." <<
         "\nSubject:\t" << *this <<
         "\nEventID:\t0x" <<
         std::setbase(16) << std::setfill('0') << std::setw(8) << eventID);

  return m_eventObservers.at(eventID);
}

bool Subject::observerIsObservingEvent(Observer& observer, EventID eventID) const
{
  auto& observersSet = getObservers(eventID);
  return observersSet.find(&observer) != observersSet.cend();
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
      CLOG(TRACE, "ObserverPattern") << "Registering Event 0x" <<
        std::setbase(16) << std::setfill('0') << std::setw(8) << registeredEvent <<
        " for Subject " << *this;
      m_eventObservers.emplace(registeredEvent, ObserversSet());
    }
  }
}
