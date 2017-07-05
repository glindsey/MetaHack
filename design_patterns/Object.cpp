#include "stdafx.h"

#include "Object.h"

#include "AssertHelper.h"
#include "Event.h"

Object::Object(std::unordered_set<EventID> const events)
  :
  m_eventQueue{},
  m_eventObservers{}
{
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
  while (!m_observations.empty())
  {
    auto iter = m_observations.begin();
    CLOG(WARNING, "EventSystem") << "Deregistering Observer " << *this << " from Subject " << *(iter->first);
    iter->first->removeObserver(*this);
  }

  bool observersRemain = false;
  for (auto& prioritizedObservers : m_eventObservers)
  {
    auto eventID = prioritizedObservers.first;
    auto observersSet = getObservers(eventID);

    if (!observersSet.empty())
    {
      CLOG(WARNING, "EventSystem") << "Subject " << *this << " was deleted while " << observersSet.size() << " objects were still observing event " << eventID;
      observersRemain = true;
    }
  }

  if (observersRemain)
  {
    CLOG(WARNING, "EventSystem") << "Clearing all observers of this subject";
    removeAllObservers();
  }
}

std::string const& Object::getName()
{
  return m_name;
}

void Object::addObserver(Object& observer, EventID eventID)
{
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

    CLOG(TRACE, "EventSystem") << "Registered Observer " << observer 
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
      observer->onEvent_NV(e);
    }

    observersSet.clear();
  }

  CLOG(TRACE, "EventSystem") << "Deregistered all Observers for all Events";
}

void Object::removeObserver(Object& observer, EventID eventID)
{
  size_t removalCount = 0;
  if (eventID == EventID::All)
  {
    for (auto& eventObservers : m_eventObservers)
    {
      auto& observersSet = eventObservers.second;
      removalCount += observersSet.erase(&observer);
    }

    CLOG(TRACE, "EventSystem") << "Deregistered Observer " << observer << " for all Events";
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

    CLOG(TRACE, "EventSystem") << "Deregistered Observer " << observer 
      << " for EventID 0x"
      << std::setbase(16) << std::setfill('0') << std::setw(8) << eventID << std::setw(0) << std::setbase(10);
  }

  if (removalCount == 0)
  {
    CLOG(WARNING, "EventSystem") 
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
  o << typeid(*this).name() << ": ";
  if (m_name.empty())
  {
    o << this;
  }
  else
  {
    o << m_name << "(" << this << ")";
  }
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
            handled = observer->onEvent_NV(*copiedEvent);
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
  auto& eventObserversIter = m_eventObservers.find(eventID);

  Assert("EventSystem", (m_eventObservers.count(eventID) != 0),
         "\nReason:\tattempted to get observers for an unregistered event." <<
         "\nSubject:\t" << *this <<
         "\nEventID:\t" << eventID);

  return m_eventObservers.at(eventID);
}

ObserversSet const& Object::getObservers(EventID eventID) const
{
  auto& eventObserversIter = m_eventObservers.find(eventID);

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