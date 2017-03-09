#include "stdafx.h"

#include "Subject.h"

#include <queue>
#include <unordered_set>

#include "AssertHelper.h"
#include "Event.h"
#include "Observer.h"

// Debugging
/// If this is defined, subject debug printouts will be emitted to std::cerr.
//#define DEBUG_SUBJECT


#ifdef DEBUG_SUBJECT
#define DEBUG_SUBJECT_PRINT(x) do { std::cerr << x << std::endl; } while(0)
#define DEBUG_SUBJECT_PRINT_EVENT(x, y) do { std::cerr << x; y->serialize(std::cerr); std::cerr << std::endl; } while(0)
#else
#define DEBUG_SUBJECT_PRINT(x)
#define DEBUG_SUBJECT_PRINT_EVENT(x, y)
#endif

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
  for (auto& eventObservers : pImpl->observers)
  {
    Assert(!eventObservers.second.size(),
           "\nReason:\tsubject went out of scope while at least one observer is still registered for one of its events" <<
           "\nSubject:\t" << this <<
           "\nObserver:\t" << *eventObservers.second.begin() <<
           "\nEvent:\t" << eventObservers.first);
  }
}

void Subject::addObserver(Observer& observer, EventID eventID)
{
  pImpl->registerEventsIfNeeded(*this);

  if (eventID == EventID::All)
  {
    for (auto& id : registeredEvents())
    {
      addObserver(observer, id);
    }
  }
  else
  {
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

    DEBUG_SUBJECT_PRINT("Registered Observer " << observer << "for EventID " << eventID);

    observer.onEvent(e);
  }
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

    DEBUG_SUBJECT_PRINT("Deegistered Observer " << observer << "for all Events");
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

    DEBUG_SUBJECT_PRINT("Deregistered Observer " << observer << "for EventID " << eventID);
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

        DEBUG_SUBJECT_PRINT_EVENT("Broadcasting: ", finalEvent);

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