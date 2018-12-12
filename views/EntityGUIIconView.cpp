#include "stdafx.h"

#include "views/EntityGUIIconView.h"

EntityGUIIconView::EntityGUIIconView(sfg::SFGUI& sfgui, EntityId entity)
  :
  EntityGUIView(sfgui, entity)
{
  /// @todo WRITE ME
  auto widget = sfg::Image::Create();

  setWidget(widget);
}

EntityGUIIconView::~EntityGUIIconView()
{}

bool EntityGUIIconView::onEvent(Event const& event)
{
  /// @todo STUFF
  return false;
}
