/// @file Observer.cpp
/// Observer implementation for observer pattern.
/// Adapted from http://0xfede.io/2015/12/13/T-C++-ObserverPattern.html


#include "stdafx.h"

#include "Observer.h"

#include "AssertHelper.h"

#include "Event.h"
#include "Subject.h"

class Observer::Impl
{

public:
  std::unordered_map<Subject*, int> observations;
};

Observer::Observer() :
  pImpl(new Impl())
{}

Observer::~Observer()
{
  for (auto& observation : pImpl->observations)
  {
    Assert("ObserverPattern", !observation.second,
           "\nReason:\tobserver went out of scope while registered with at least one subject." <<
           "\nSubject:\t" << observation.first <<
           "\nObserver:\t" << *this);
  }
}

bool Observer::onEvent(Event const& event)
{
  if (event.getId() == Subject::Registration::id())
  {
    auto e = static_cast<const Subject::Registration&>(event);
    if (e.state == Subject::Registration::State::Registered)
    {
      ++(pImpl->observations[e.subject]);
    }
    else if (e.state == Subject::Registration::State::Unregistered)
    {
      if (!--pImpl->observations[e.subject])
      {
        pImpl->observations.erase(e.subject);
      }
    }

    return true;
  }

  auto eventResults = onEvent_NVI(event);

  if (eventResults.event_handled == EventHandled::No)
  {
    Assert("ObserverPattern", false,
           "\nReason:\tobserver did not handle event it is subscribed to." <<
           "\nSubject:\t" << *event.subject <<
           "\nObserver:\t" << *this <<
           "\nEvent:\t" << event);
  }

  return{ eventResults.continue_broadcasting == ContinueBroadcasting::Yes };
}

  