#include "stdafx.h"

#include "views/EntityGUIView.h"

#include "game/App.h"
#include "game/GameState.h"
#include "entity/EntityFactory.h"

EntityGUIView::EntityGUIView(sfg::SFGUI& sfgui, EntityId entity)
  :
  Object({}),
  m_sfgui{ sfgui },
  m_entityId{ entity }
{}

sfg::Widget::Ptr EntityGUIView::getWidget() const
{
  return m_widget;
}

EntityGUIView::~EntityGUIView()
{}

void EntityGUIView::setWidget(sfg::Widget::Ptr widget)
{
  m_widget = widget;
}

EntityId EntityGUIView::getEntity() const
{
  return m_entityId;
}
