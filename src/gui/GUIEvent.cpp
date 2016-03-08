#include "stdafx.h"

#include "gui/GUIEvent.h"

namespace metagui
{
  Event Event::create(sf::Event event)
  {
    switch (event.type)
    {
      /// @todo WRITE ME
      default:
        return Event{ Type::Unknown, {} };
    }
  }

  /// Virtual destructor.
  Event::~Event()
  {}

  Event::Event(Type type, boost::any data)
    :
    m_type{ type },
    m_data{ data }
  {}
}; // end namespace metagui