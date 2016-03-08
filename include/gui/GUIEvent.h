#pragma once

#include "stdafx.h"

namespace metagui
{
  /// Superclass for MetaGUI events.
  /// This is not the most elegant implementation, but it works well enough and
  /// is similar to the SFML implementation of sf::Event.
  class Event
  {
    enum class Type
    {
      Unknown,
      MemberCount
    };

  public:
    /// Static factory function that takes an SFML Event and outputs the
    /// requested MetaGUI event.
    static Event create(sf::Event event);

    /// Virtual destructor.
    virtual ~Event();

  protected:
    /// Constructor function that takes an event type, and a boost::any
    /// containing the appropriate information.
    Event(Type type, boost::any data);

    /// Template function to set the event data.
    template< typename T >
    void set_data(T data)
    {
      m_data = boost::any(data);
    }

    /// Template function to get the event data.
    template< typename T >
    T get_data()
    {
      return boost::any_cast<T>(m_data);
    }

  private:
    /// Type of this event.
    Type m_type;

    /// Data for this event.
    boost::any m_data;
  };
}; // end namespace metagui
