#ifndef SFMLEVENTHANDLER_H
#define SFMLEVENTHANDLER_H

#include <SFML/Graphics.hpp>

#include "types/SFMLEventResult.h"

/// Interface class indicating that an object handles SFML events.
class ISFMLEventHandler
{
public:
  virtual ~ISFMLEventHandler() {}
  virtual SFMLEventResult handle_sfml_event(sf::Event& event) = 0;

protected:
private:
};

#endif // EVENTHANDLER_H
