#include "stdafx.h"

#include "Object.h"

#include <boost/core/demangle.hpp>
#include <typeinfo>

#include "AssertHelper.h"
#include "Event.h"
#include "ObjectRegistry.h"

Object::Object(std::unordered_set<EventID> const events)
  :
  m_eventQueue{},
  m_eventObservers{}
{
  REGISTRY.add(this);

  for (EventID const event : events)
  {
    CLOG(TRACE, "EventSystem") << "Registering Event 0x" <<
      std::setbase(16) << std::setfill('0') << std::setw(8) << event <<
      " for Object " << *this << std::setw(0) << std::setbase(10);
    m_eventObservers.emplace(event, ObserversSet());
  }
}

Object::Object(std::unordered_set<EventID> const events, std::string name) :
  Object(events)
{
  m_name = name;
}

Object::~Object()
{
  CLOG(TRACE, "EventSystem") << "Destroying Object " << *this;
  CLOG(TRACE, "EventSystem") << "Removing from observed Subjects";
  while (!m_observations.empty())
  {
    auto iter = m_observations.begin();
    if (REGISTRY.contains(iter->first))
    {
      CLOG(TRACE, "EventSystem") << "Removing from Subject " << *(iter->first);
      iter->first->removeObserver(*this);
    }
    else
    {
      CLOG(TRACE, "EventSystem") << "Subject " << *(iter->first) << " already deleted, skipping";
    }
  }

  removeAllObservers();

  REGISTRY.remove(this);
}

std::string const& Object::getName()
{
  return m_name;
}

void Object::addObserver(Object& observer, EventID eventID)
{
  if (!REGISTRY.contains(&observer))
  {
    CLOG(WARNING, "EventSystem") << "Attempted to add destroyed Observer " << observer
      << " for EventID 0x"
      << std::setbase(16) << std::setfill('0') << std::setw(8) << eventID << std::setw(0) << std::setbase(10)
      << ", skipping";

    return;
  }

  if (eventID == EventID::All)
  {
    for (auto& pair : m_eventObservers)
    {
      addObserver(observer, pair.first);
    }
  }
  else
  {
    Assert("EventSystem", !observerIsObservingEvent(observer, eventID),
           "\nReason:\tattempted to add an observer for an event it is already observing." <<
           "\nSubject:\t" << *this <<
           "\nObserver:\t" << observer <<
           "\nEvent:\t" << eventID);

    auto& observersSet = getObservers(eventID);
    observersSet.insert(&observer);

    Registration e;
    e.state = Registration::State::Registered;
    e.subject = this;

    CLOG(TRACE, "EventSystem") << "Added Observer " << observer
      << " for EventID 0x"
      << std::setbase(16) << std::setfill('0') << std::setw(8) << eventID << std::setw(0) << std::setbase(10);

    observer.onEvent_NV(e);
  }
}

void Object::removeAllObservers()
{
  Registration e;
  e.state = Registration::State::Unregistered;
  e.subject = this;

  for (auto& eventObservers : m_eventObservers)
  {
    auto& observersSet = eventObservers.second;
    for (auto& observer : observersSet)
    {
      if (REGISTRY.contains(observer))
      {
        observer->onEvent_NV(e);
      }
    }

    observersSet.clear();
  }

  CLOG(TRACE, "EventSystem") << "Removed all Observers of " << *this << " for all Events";
}

void Object::removeObserver(Object& observer, EventID eventID)
{
  if (!REGISTRY.contains(&observer))
  {
    CLOG(TRACE, "EventSystem") << "Attempted to remove destroyed Observer " << observer
      << " for EventID 0x"
      << std::setbase(16) << std::setfill('0') << std::setw(8) << eventID << std::setw(0) << std::setbase(10)
      << ", ignoring";

    return;
  }

  size_t removalCount = 0;
  if (eventID == EventID::All)
  {
    for (auto& eventObservers : m_eventObservers)
    {
      auto& observersSet = eventObservers.second;
      removalCount += observersSet.erase(&observer);
    }

    CLOG(TRACE, "EventSystem") << "Removed Observer " << observer << " for all Events";
  }
  else
  {
    auto& observersSet = getObservers(eventID);

    Assert("EventSystem", (observersSet.size() > 0),
           "\nReason:\tattempted to remove an observer on event with no observers." <<
           "\nSubject:\t" << *this <<
           "\nObserver:\t" << observer <<
           "\nEvent:\t" << eventID);

    removalCount = observersSet.erase(&observer);

    CLOG(TRACE, "EventSystem") << "Removed Observer " << observer
      << " for EventID 0x"
      << std::setbase(16) << std::setfill('0') << std::setw(8) << eventID << std::setw(0) << std::setbase(10);
  }

  if (removalCount == 0)
  {
    std::stringstream ss;
    ss << "Event " << eventID;

    CLOG(TRACE, "EventSystem")
      << "Tried to remove Observer " << observer
      << " of Subject " << *this << ", but it was not registered for "
      << ((eventID == EventID::All) ? "any Events" : ss.str());
  }

  Registration e;
  e.state = Registration::State::Unregistered;
  e.subject = this;

  for (int i = 0; i < removalCount; i++)
  {
    observer.onEvent_NV(e);
  }
}

bool Object::onEvent_NV(Event const& event)
{
  if (event.getId() == Object::Registration::id)
  {
    auto e = static_cast<const Object::Registration&>(event);
    if (e.state == Object::Registration::State::Registered)
    {
      ++(m_observations[e.subject]);
    }
    else if (e.state == Object::Registration::State::Unregistered)
    {
      if (!--m_observations[e.subject])
      {
        m_observations.erase(e.subject);
      }
    }

    return true;
  }

  bool result = onEvent(event);

  return result;
}

bool Object::onEvent(Event const& event)
{
  return false;
}

void Object::serialize(std::ostream & o) const
{
  auto mangledName = typeid(*this).name();
  auto demangledName = boost::core::demangle(mangledName);

  o << demangledName << ": ";
  o << this << " (" << (m_name.empty() ? "<unnamed>" : m_name) << ")";
}

bool Object::broadcast(Event& event)
{
  bool result = true;

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
        CLOG(TRACE, "EventSystem") << "Broadcasting: " << *copiedEvent;

        auto id = copiedEvent->getId();
        auto& observersSet = getObservers(id);

        // Return if no observers for this event.
        if (observersSet.size() == 0)
        {
          CLOG(TRACE, "EventSystem") << "No observers registered for event";
          handled = true;
        }
        else
        {
          for (auto& observer : observersSet)
          {
            if (!REGISTRY.contains(observer))
            {
              CLOG(TRACE, "EventSystem") << "Attempted to broadcast to destroyed Observer " << observer << ", skipping";
              handled = true;
            }
            else
            {
              handled = observer->onEvent_NV(*copiedEvent);
            }

            if (handled)
            {
              CLOG(TRACE, "EventSystem") << "Broadcast Halted: " << *copiedEvent << " by " << *observer;
              break;
            }
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

void Object::unicast(Event& event, Object& observer)
{
  event.subject = this;

  unicast_(event, observer, [&](Event& event, Object& observer, bool shouldSend)
  {
    if (shouldSend)
    {
      if (!REGISTRY.contains(&observer))
      {
        CLOG(TRACE, "EventSystem") << "Attempted to unicast to destroyed Observer " << observer << ", skipping";
        return;
      }

      auto finalEvent = event.heapClone();

      auto dispatchBlock = [=, &observer]() mutable -> bool
      {
        bool result = true;

        if (observerIsObservingEvent(observer, finalEvent->getId()))
        {
          CLOG(TRACE, "EventSystem") << "Unicasting " << *finalEvent << " to " << observer;
          result = observer.onEvent_NV(*finalEvent);
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

ObserversSet& Object::getObservers(EventID eventID)
{
  //auto eventObserversIter = m_eventObservers.find(eventID);

  Assert("EventSystem", (m_eventObservers.count(eventID) != 0),
         "\nReason:\tattempted to get observers for an unregistered event." <<
         "\nSubject:\t" << *this <<
         "\nEventID:\t" << eventID);

  return m_eventObservers.at(eventID);
}

ObserversSet const& Object::getObservers(EventID eventID) const
{
  //auto eventObserversIter = m_eventObservers.find(eventID);

  Assert("EventSystem", (m_eventObservers.count(eventID) != 0),
         "\nReason:\tattempted to get observers for an unregistered event." <<
         "\nSubject:\t" << *this <<
         "\nEventID:\t" << eventID);

  return m_eventObservers.at(eventID);
}

bool Object::observerIsObservingEvent(Object& observer, EventID eventID) const
{
  auto& observersSet = getObservers(eventID);
  return observersSet.find(&observer) != observersSet.cend();
}

bool Object::broadcast_(Event& event, BroadcastDelegate do_broadcast)
{
  return do_broadcast(event, true);
}

void Object::unicast_(Event& event, Object& observer, UnicastDelegate do_unicast)
{
  do_unicast(event, observer, true);
}

void Object::Registration::serialize(std::ostream& o) const
{
  Event::serialize(o);
  o << " | registration state: " <<
    (state == State::Registered ? "Registered" : "Unregistered");
}
