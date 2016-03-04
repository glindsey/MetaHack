#include "stdafx.h"

#include "gui/GUIPane.h"

#include "App.h"
#include "ConfigSettings.h"
#include "New.h"

GUIPane::GUIPane(std::string name, sf::Vector2i location, sf::Vector2i size)
  :
  GUIObject(name, location, size)
{}

GUIPane::GUIPane(std::string name, sf::IntRect dimensions)
  :
  GUIObject(name, dimensions)
{}

GUIPane::~GUIPane()
{}

// === PROTECTED METHODS ======================================================
