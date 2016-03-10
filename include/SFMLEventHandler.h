#ifndef SFMLEVENTHANDLER_H
#define SFMLEVENTHANDLER_H

#include "stdafx.h"

// Interface class indicating that an object handles SFML events.
class SFMLEventHandler
{
public:
  virtual ~SFMLEventHandler() {}
  virtual SFMLEventResult handle_sfml_event(sf::Event& event) = 0;

protected:
private:
};

#endif // EVENTHANDLER_H
