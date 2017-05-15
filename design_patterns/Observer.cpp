/// @file Observer.cpp
/// Observer implementation for observer pattern.
/// Adapted from http://0xfede.io/2015/12/13/T-C++-ObserverPattern.html


#include "stdafx.h"

#include "Observer.h"

#include "AssertHelper.h"

#include "Event.h"
#include "Subject.h"

Observer::Observer()
{}

Observer::~Observer()
{
  for (auto& observation : m_observations)
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
      ++(m_observations[e.subject]);
    }
    else if (e.state == Subject::Registration::State::Unregistered)
    {
      if (!--m_observations[e.subject])
      {
        m_observations.erase(e.subject);
      }
    }

    return true;
  }

  bool result = onEvent_V(event);

  return result;
}

  