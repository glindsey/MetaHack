#include "stdafx.h"

#include "gui/GUIPane.h"

#include "App.h"
#include "ConfigSettings.h"
#include "New.h"

namespace metagui
{
  Pane::Pane(std::string name, sf::Vector2i location, sf::Vector2u size)
    :
    Object(name, location, size)
  {}

  Pane::Pane(std::string name, sf::IntRect dimensions)
    :
    Object(name, dimensions)
  {}

  Pane::~Pane()
  {}
}; // end namespace metagui

// === PROTECTED METHODS ======================================================
